#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>

using namespace std;

// --- 全局常量和类型定义 ---
const int WARRIOR_TYPES = 5;
const string WARRIOR_NAMES[WARRIOR_TYPES] = {"dragon", "ninja", "iceman", "lion", "wolf"};
const int RED_PRODUCE_ORDER[WARRIOR_TYPES] = {2, 3, 4, 1, 0}; // iceman, lion, wolf, ninja, dragon
const int BLUE_PRODUCE_ORDER[WARRIOR_TYPES] = {3, 0, 1, 2, 4}; // lion, dragon, ninja, iceman, wolf

// --- 数据结构 ---
struct Army {
    int id;
    int type; // 0-dragon, 1-ninja, 2-iceman, 3-lion, 4-wolf
    int color; // 0-red, 1-blue
    int elements;
    int force;
    int location; // 0 for red hq, N+1 for blue hq, 1..N for cities
    int kill_count;
    int steps; // for iceman

    Army(int _id, int _type, int _color, int _elements, int _force, int _location)
        : id(_id), type(_type), color(_color), elements(_elements), force(_force), location(_location), kill_count(0), steps(0) {}
};

struct Base {
    int money;
    int color; // 0-red, 1-blue
    int next_warrior_type_idx;
    vector<Army> armies_in_hq; // 武士在司令部里
    vector<Army> enemy_armies; // 进入司令部的敌人

    Base(int _color, int initial_money) : color(_color), money(initial_money), next_warrior_type_idx(0) {}
};

struct City {
    int flag; // -1: none, 0: red, 1: blue
    int elements;
    vector<Army> red_army;
    vector<Army> blue_army;
    int last_winner; // -1: none, 0: red, 1: blue
    int win_count;

    City() : flag(-1), elements(0), last_winner(-1), win_count(0) {}
};

// --- 全局变量 ---
vector<string> events;
int initial_life_values[WARRIOR_TYPES];
int initial_force[WARRIOR_TYPES];
int next_red_id = 1;
int next_blue_id = 1;
bool game_over = false;
int N; // 城市数量

// --- 辅助函数 ---
string format_time(int t) {
    int hour = t / 60;
    int minute = t % 60;
    stringstream ss;
    ss << setw(3) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << " ";
    return ss.str();
}

string get_color_str(int color) {
    return color == 0 ? "red" : "blue";
}

// --- 事件生成函数 ---
void add_event(int t, const string& event) {
    events.push_back(format_time(t) + event);
}

// --- 核心逻辑函数 ---
void produce_warriors(int t, Base& hq, const int* produce_order) {
    if (game_over) return;
    
    int produced = 0;
    for (int i = 0; i < WARRIOR_TYPES; ++i) {
        int type_idx = (hq.next_warrior_type_idx + i) % WARRIOR_TYPES;
        int warrior_type = produce_order[type_idx];
        int cost = initial_life_values[warrior_type];

        if (hq.money >= cost) {
            hq.money -= cost;
            int id = (hq.color == 0) ? next_red_id++ : next_blue_id++;
            Army new_army(id, warrior_type, hq.color, cost, initial_force[warrior_type], 0);
            hq.armies_in_hq.push_back(new_army);
            add_event(t, get_color_str(hq.color) + " " + WARRIOR_NAMES[warrior_type] + " " + to_string(id) + " born");
            hq.next_warrior_type_idx = (type_idx + 1) % WARRIOR_TYPES;
            produced = 1;
            break;
        }
    }
    if (!produced) {
        hq.next_warrior_type_idx = (hq.next_warrior_type_idx + 1) % WARRIOR_TYPES;
    }
}

void warriors_move(int t, vector<City>& cities, Base& red_hq, Base& blue_hq) {
    if (game_over) return;

    // 临时存储移动后的状态
    vector<City> next_cities = cities;
    vector<Army> next_red_hq_enemies = red_hq.enemy_armies;
    vector<Army> next_blue_hq_enemies = blue_hq.enemy_armies;

    // 清空当前状态
    for(auto& city : next_cities) {
        city.red_army.clear();
        city.blue_army.clear();
    }
    next_red_hq_enemies.clear();
    next_blue_hq_enemies.clear();

    // 处理从城市出发的武士
    for (int i = 0; i < N; ++i) {
        City& city = cities[i];
        // Red army moves
        if (!city.red_army.empty()) {
            Army a = city.red_army[0];
            a.location++;
            a.steps++;
            // iceman special ability
            if (a.type == 2 && a.steps % 2 == 0) {
                a.force += 20;
                if (a.elements > 9) a.elements -= 9;
                else a.elements = 1;
            }
            if (a.location == N + 1) { // Reached blue hq
                add_event(t, get_color_str(a.color) + " " + WARRIOR_NAMES[a.type] + " " + to_string(a.id) + " reached blue headquarter with " + to_string(a.elements) + " elements and force " + to_string(a.force));
                next_blue_hq_enemies.push_back(a);
            } else {
                next_cities[a.location - 1].red_army.push_back(a);
                add_event(t, get_color_str(a.color) + " " + WARRIOR_NAMES[a.type] + " " + to_string(a.id) + " marched to city " + to_string(a.location) + " with " + to_string(a.elements) + " elements and force " + to_string(a.force));
            }
        }
        // Blue army moves
        if (!city.blue_army.empty()) {
            Army a = city.blue_army[0];
            a.location--;
            a.steps++;
            // iceman special ability
            if (a.type == 2 && a.steps % 2 == 0) {
                a.force += 20;
                if (a.elements > 9) a.elements -= 9;
                else a.elements = 1;
            }
            if (a.location == 0) { // Reached red hq
                add_event(t, get_color_str(a.color) + " " + WARRIOR_NAMES[a.type] + " " + to_string(a.id) + " reached red headquarter with " + to_string(a.elements) + " elements and force " + to_string(a.force));
                next_red_hq_enemies.push_back(a);
            } else {
                next_cities[a.location - 1].blue_army.push_back(a);
                add_event(t, get_color_str(a.color) + " " + WARRIOR_NAMES[a.type] + " " + to_string(a.id) + " marched to city " + to_string(a.location) + " with " + to_string(a.elements) + " elements and force " + to_string(a.force));
            }
        }
    }
    
    // 处理从司令部出发的武士
    if (!red_hq.armies_in_hq.empty()) {
        Army a = red_hq.armies_in_hq[0];
        a.location = 1;
        a.steps = 1;
        next_cities[0].red_army.push_back(a);
        add_event(t, get_color_str(a.color) + " " + WARRIOR_NAMES[a.type] + " " + to_string(a.id) + " marched to city 1 with " + to_string(a.elements) + " elements and force " + to_string(a.force));
    }
    if (!blue_hq.armies_in_hq.empty()) {
        Army a = blue_hq.armies_in_hq[0];
        a.location = N;
        a.steps = 1;
        next_cities[N-1].blue_army.push_back(a);
        add_event(t, get_color_str(a.color) + " " + WARRIOR_NAMES[a.type] + " " + to_string(a.id) + " marched to city " + to_string(N) + " with " + to_string(a.elements) + " elements and force " + to_string(a.force));
    }

    // 更新状态
    cities = next_cities;
    red_hq.enemy_armies = next_red_hq_enemies;
    blue_hq.enemy_armies = next_blue_hq_enemies;
    red_hq.armies_in_hq.clear();
    blue_hq.armies_in_hq.clear();

    // Check for game over
    if (red_hq.enemy_armies.size() >= 2) {
        add_event(t, "red headquarter was taken");
        game_over = true;
    }
    if (blue_hq.enemy_armies.size() >= 2) {
        add_event(t, "blue headquarter was taken");
        game_over = true;
    }
}

void cities_produce(int t, vector<City>& cities) {
    if (game_over) return;
    for (auto& city : cities) {
        city.elements += 10;
    }
}

void warriors_collect(int t, vector<City>& cities, Base& red_hq, Base& blue_hq) {
    if (game_over) return;
    for (int i = 0; i < N; ++i) {
        City& city = cities[i];
        if (city.elements > 0) {
            if (city.red_army.size() == 1 && city.blue_army.empty()) {
                red_hq.money += city.elements;
                add_event(t, get_color_str(0) + " " + WARRIOR_NAMES[city.red_army[0].type] + " " + to_string(city.red_army[0].id) + " earned " + to_string(city.elements) + " elements for his headquarter");
                city.elements = 0;
            } else if (city.blue_army.size() == 1 && city.red_army.empty()) {
                blue_hq.money += city.elements;
                add_event(t, get_color_str(1) + " " + WARRIOR_NAMES[city.blue_army[0].type] + " " + to_string(city.blue_army[0].id) + " earned " + to_string(city.elements) + " elements for his headquarter");
                city.elements = 0;
            }
        }
    }
}

void battle_happen(int t, vector<City>& cities, Base& red_hq, Base& blue_hq) {
    if (game_over) return;

    vector<pair<Army*, int>> reward_candidates; // For sorting rewards

    for (int i = 0; i < N; ++i) {
        City& city = cities[i];
        if (!city.red_army.empty() && !city.blue_army.empty()) {
            Army* red = &city.red_army[0];
            Army* blue = &city.blue_army[0];
            Army* attacker, *defender;
            int attacker_color;

            // Determine attacker
            if (city.flag == 0 || (city.flag == -1 && (i + 1) % 2 == 1)) {
                attacker = red; defender = blue; attacker_color = 0;
            } else {
                attacker = blue; defender = red; attacker_color = 1;
            }

            int defender_pre_hp = defender->elements;
            // 修正点1: WARRIOR_NAMES[defender->type] 而不是 WARRIOR_NAMES[defender->type)
            add_event(t, get_color_str(attacker->color) + " " + WARRIOR_NAMES[attacker->type] + " " + to_string(attacker->id) + " attacked " + get_color_str(defender->color) + " " + WARRIOR_NAMES[defender->type] + " " + to_string(defender->id) + " in city " + to_string(i + 1) + " with " + to_string(attacker->elements) + " elements and force " + to_string(attacker->force));

            // Attack
            defender->elements -= attacker->force;
            bool defender_dead = defender->elements <= 0;
            bool attacker_dead = false;

            // Fight back
            if (!defender_dead && defender->type != 1) { // Ninja never fights back
                // 修正点2: WARRIOR_NAMES[attacker->type] 而不是 WARRIOR_NAMES[attacker->type)
                add_event(t, get_color_str(defender->color) + " " + WARRIOR_NAMES[defender->type] + " " + to_string(defender->id) + " fought back against " + get_color_str(attacker->color) + " " + WARRIOR_NAMES[attacker->type] + " " + to_string(attacker->id) + " in city " + to_string(i + 1));
                attacker->elements -= defender->force / 2;
                attacker_dead = attacker->elements <= 0;
            }

            // Process results
            Army* winner = nullptr;
            int winner_color = -1;
            if (defender_dead && !attacker_dead) {
                winner = attacker; winner_color = attacker_color;
                add_event(t, get_color_str(defender->color) + " " + WARRIOR_NAMES[defender->type] + " " + to_string(defender->id) + " was killed in city " + to_string(i + 1));
            } else if (attacker_dead && !defender_dead) {
                winner = defender; winner_color = 1 - attacker_color;
                add_event(t, get_color_str(attacker->color) + " " + WARRIOR_NAMES[attacker->type] + " " + to_string(attacker->id) + " was killed in city " + to_string(i + 1));
            } else if (attacker_dead && defender_dead) {
                add_event(t, get_color_str(attacker->color) + " " + WARRIOR_NAMES[attacker->type] + " " + to_string(attacker->id) + " was killed in city " + to_string(i + 1));
                add_event(t, get_color_str(defender->color) + " " + WARRIOR_NAMES[defender->type] + " " + to_string(defender->id) + " was killed in city " + to_string(i + 1));
            }

            if (winner) {
                // Wolf's special ability (must be active attacker)
                if (winner->type == 4 && winner == attacker) {
                    winner->kill_count++;
                    if (winner->kill_count % 2 == 0) {
                        winner->elements *= 2;
                        winner->force *= 2;
                    }
                }
                
                // Lion's special ability
                if (defender->type == 3) {
                    winner->elements += defender_pre_hp;
                }

                // Reward and Spoils
                reward_candidates.push_back({winner, winner_color});
                
                // Update flag
                if (city.last_winner == winner_color) {
                    city.win_count++;
                    if (city.win_count >= 2) {
                        city.flag = winner_color;
                        add_event(t, get_color_str(winner_color) + " flag raised in city " + to_string(i + 1));
                    }
                } else {
                    city.last_winner = winner_color;
                    city.win_count = 1;
                }
            } else { // Draw or both dead
                city.last_winner = -1;
                city.win_count = 0;
                // Dragon's yell if it attacked and survived
                if (attacker->type == 0 && !attacker_dead) {
                    add_event(t, get_color_str(attacker->color) + " " + WARRIOR_NAMES[attacker->type] + " " + to_string(attacker->id) + " yelled in city " + to_string(i + 1));
                }
            }
        }
    }

    // Give rewards
    if (!reward_candidates.empty()) {
        // Sort by distance to enemy HQ
        sort(reward_candidates.begin(), reward_candidates.end(), [](const auto& a, const auto& b) {
            int dist_a = (a.second == 0) ? (N + 1 - a.first->location) : a.first->location;
            int dist_b = (b.second == 0) ? (N + 1 - b.first->location) : b.first->location;
            if (dist_a != dist_b) return dist_a < dist_b;
            return a.first->id < b.first->id; // Tie-breaker
        });

        for (auto& p : reward_candidates) {
            Army* winner = p.first;
            Base& hq = (winner->color == 0) ? red_hq : blue_hq;
            if (hq.money >= 8) {
                hq.money -= 8;
                winner->elements += 8;
                add_event(t, get_color_str(winner->color) + " " + WARRIOR_NAMES[winner->type] + " " + to_string(winner->id) + " earned 8 elements for his headquarter");
            }
        }
        
        // Collect spoils
        for (auto& p : reward_candidates) {
            Army* winner = p.first;
            City& city = cities[winner->location - 1];
            Base& hq = (winner->color == 0) ? red_hq : blue_hq;
            if (city.elements > 0) {
                hq.money += city.elements;
                city.elements = 0;
            }
        }
    }
    
    // Remove dead armies
    for (auto& city : cities) {
        if (!city.red_army.empty() && city.red_army[0].elements <= 0) city.red_army.clear();
        if (!city.blue_army.empty() && city.blue_army[0].elements <= 0) city.blue_army.clear();
    }
}

void report(int t, Base& red_hq, Base& blue_hq) {
    if (game_over) return;
    add_event(t, to_string(red_hq.money) + " elements in red headquarter");
    add_event(t, to_string(blue_hq.money) + " elements in blue headquarter");
}


int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int test_cases;
    cin >> test_cases;

    for (int case_num = 1; case_num <= test_cases; ++case_num) {
        // --- Reset global state ---
        events.clear();
        next_red_id = 1;
        next_blue_id = 1;
        game_over = false;

        // --- Input ---
        int M, T;
        cin >> M >> N >> T;
        for (int i = 0; i < WARRIOR_TYPES; ++i) cin >> initial_life_values[i];
        for (int i = 0; i < WARRIOR_TYPES; ++i) cin >> initial_force[i];

        // --- Initialization ---
        Base red_hq(0, M);
        Base blue_hq(1, M);
        vector<City> cities(N);

        // --- Simulation Loop ---
        for (int t = 0; t <= T && !game_over; ++t) {
            int minute = t % 60;
            if (minute == 0) {
                produce_warriors(t, red_hq, RED_PRODUCE_ORDER);
                produce_warriors(t, blue_hq, BLUE_PRODUCE_ORDER);
            } else if (minute == 10) {
                warriors_move(t, cities, red_hq, blue_hq);
            } else if (minute == 20) {
                cities_produce(t, cities);
            } else if (minute == 30) {
                warriors_collect(t, cities, red_hq, blue_hq);
            } else if (minute == 40) {
                battle_happen(t, cities, red_hq, blue_hq);
            } else if (minute == 50) {
                report(t, red_hq, blue_hq);
            }
        }

        // --- Output ---
        cout << "Case:" << case_num << "\n";
        for (const string& event : events) {
            cout << event << "\n";
        }
    }

    return 0;
}
