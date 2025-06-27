#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <map>
#include <conio.h>
#include <windows.h>
#include <locale>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <set>
#include <typeinfo>
#include <iomanip>
#include <fstream> // 用于保存历史数据

using namespace std;

// 设置控制台编码为GB2312以支持中文
void SetConsoleToGB2312() {
    SetConsoleOutputCP(936); // GB2312编码
    SetConsoleCP(936);
}

// 控制台颜色枚举
enum ConsoleColors {
    COLOR_DEFAULT = 7,
    COLOR_PLANT = 10,
    COLOR_HERBIVORE = 14,
    COLOR_CARNIVORE = 12,
    COLOR_WATER = 9,
    COLOR_MOUNTAIN = 8,
    COLOR_DESERT = 6,
    COLOR_FOREST = 2,
    COLOR_MARSH = 11,
    COLOR_VOLCANIC = 4,
    COLOR_SNOW = 15,
    COLOR_STATS = 11,
    COLOR_TITLE = 14,
    COLOR_WARNING = 12,
    COLOR_HIGHLIGHTA = 13,
    COLOR_GRASSLAND = 10,
    COLOR_JUNGLE = 2,
    COLOR_TUNDRA = 11,
    COLOR_BEACH = 6,
    COLOR_RAIN = 9,
    COLOR_DROUGHT = 6
};

// 设置控制台颜色
void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 天气类型枚举
enum WeatherType {
    SUNNY,      // 晴天
    RAINY,      // 雨天
    SNOWY,      // 雪天
    CLOUDY,     // 多云
    STORMY,     // 暴风雨
    DROUGHT     // 干旱
};

// 地形类型枚举
enum TerrainType {
    PLAIN,      // 平原
    FOREST,     // 森林
    MOUNTAIN,   // 山脉
    DESERT,     // 沙漠
    WATER,      // 水域
    MARSH,      // 沼泽
    VOLCANIC,   // 火山
    SNOW,       // 雪地
    GRASSLAND,  // 草原
    JUNGLE,     // 丛林
    TUNDRA,     // 苔原
    BEACH,      // 海滩
    FLOODED     // 积水区
};

// 疾病类型
enum DiseaseType {
    NONE,
    FUNGAL_INFECTION,  // 真菌感染
    VIRAL_OUTBREAK,    // 病毒爆发
    PARASITIC_INFESTATION // 寄生虫感染
};

// 地形结构体
struct Terrain {
    TerrainType type;
    double height;      // 高度
    double fertility;   // 肥沃度
    double water_level; // 水位/湿度
    double pollution_level; // 污染程度
    double disease_level;   // 疾病程度
    double water_accumulation; // 积水深度 (0-1.0)
    double snow_depth;       // 积雪深度 (0-1.0)
    double drought_level;    // 干旱程度 (0-1.0)
    double original_height;  // 原始高度，用于洪水退去后恢复地形

    Terrain() : type(PLAIN), height(0.0), fertility(0.5), water_level(0.5),
        pollution_level(0.0), disease_level(0.0), water_accumulation(0.0),
        snow_depth(0.0), drought_level(0.0), original_height(0.0) {
    }
};

// 环境参数结构体
struct Environment {
    double temperature;    // 温度 (-20-50)
    double humidity;       // 湿度 (0-100)
    double disaster_chance;// 灾难概率 (0-0.1)
    double pollution;      // 污染程度 (0-1.0)
    double season_progress;// 季节进度 (0-1)
    double rainfall;       // 降雨量 (0-100)
    double daylight_hours; // 白天时长 (0-24)
    DiseaseType disease;   // 当前流行疾病
    int disease_duration;  // 疾病持续时间
    WeatherType weather;   // 当前天气
    int weather_duration;  // 天气持续时间
    int consecutive_rain;  // 连续降雨天数
    int consecutive_sunny; // 连续晴天天数

    Environment() : temperature(25.0), humidity(50.0),
        disaster_chance(0.01), pollution(0.1), season_progress(0.0),
        rainfall(0.0), daylight_hours(12.0), disease(NONE), disease_duration(0),
        weather(SUNNY), weather_duration(1), consecutive_rain(0), consecutive_sunny(0) {
    }
};

// 前向声明所有生物类
class Plant;
class Herbivore;
class Carnivore;
class Omnivore;
class Insect;
class Decomposer;
class ApexPredator;
class Parasite;
class Fish;
class Bird;
class Reptile;
class Amphibian;
class Scavenger;

// 生物基类
class Organism {
protected:
    int x, y;             // 位置
    double energy;         // 能量值
    int age;               // 年龄
    int max_age;           // 最大寿命
    double reproduction_threshold; // 繁殖所需能量阈值
    double reproduction_chance;    // 繁殖概率
    double mobility;       // 移动能力
    double preferred_temp; // 偏好温度
    double temp_tolerance; // 温度耐受范围
    bool is_aquatic;       // 是否水生
    bool is_hibernating;   // 是否在冬眠
    bool has_disease;      // 是否患病
    int disease_resistance; // 疾病抵抗力 (0-100)
    int territory_size;    // 领地大小
    int days_without_food; // 饥饿天数
    double flood_resistance; // 抗洪能力 (0-1.0)
    double drought_resistance; // 抗旱能力 (0-1.0)
    double base_energy;    // 基础能量消耗

    // 前一天的状态
    struct PreviousState {
        int x, y;
        double energy;
        int age;
        bool is_dead;
        string status;
    } previous;

public:
    Organism(int x, int y, double energy)
        : x(x), y(y), energy(energy), age(0), max_age(0),
        reproduction_threshold(0.0), reproduction_chance(0.3),
        mobility(1.0), preferred_temp(25.0), temp_tolerance(15.0),
        is_aquatic(false), is_hibernating(false), has_disease(false),
        disease_resistance(50), territory_size(1), days_without_food(0),
        flood_resistance(0.2), drought_resistance(0.5), base_energy(0.1) {
        save_previous_state("创建");
    }

    virtual ~Organism() {}

    // 保存前一天状态
    void save_previous_state(const string& status) {
        previous.x = x;
        previous.y = y;
        previous.energy = energy;
        previous.age = age;
        previous.is_dead = (energy <= 0 || age >= max_age);
        previous.status = status;
    }

    // 获取前一天状态
    const PreviousState& get_previous_state() const {
        return previous;
    }

    // 纯虚函数 - 需要在子类实现
    virtual void move(vector<vector<Terrain>>& terrain, Environment& env) = 0;
    virtual void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) = 0;
    virtual Organism* reproduce() = 0;
    virtual string getSymbol() const = 0;
    virtual string getName() const = 0;
    virtual bool canInhabit(TerrainType type) const = 0;
    virtual void seasonal_effect(Environment& env) {}  // 添加默认实现
    virtual void weather_effect(Environment& env, Terrain& terrain) {} // 天气影响

    // 疾病相关函数
    virtual void contract_disease(DiseaseType disease_type) {
        if (rand() % 100 > disease_resistance) {
            has_disease = true;
            save_previous_state("感染疾病");
        }
    }

    // 修改spread_disease函数签名
    virtual void spread_disease(vector<Organism*>& nearby_organisms, DiseaseType disease_type) {
        if (has_disease && rand() % 100 < 30) { // 30%几率传播疾病
            for (Organism* org : nearby_organisms) {
                // 使用typeid进行类型检查
                if (org != this && typeid(*org) == typeid(*this)) {
                    org->contract_disease(disease_type);
                }
            }
        }
    }

    virtual void disease_effects() {
        if (!has_disease) return;

        // 疾病影响
        lose_energy(0.5); // 每天损失能量
        mobility *= 0.7;   // 移动能力降低

        // 小概率死亡
        if (rand() % 100 < 5) {
            lose_energy(energy); // 直接死亡
        }

        // 小概率康复
        if (rand() % 100 < disease_resistance / 10) {
            has_disease = false;
            save_previous_state("康复");
        }
    }

    // 通用函数
    void age_organism(Environment& env) {
        age++;
        // 基础能量消耗（与体型和活动相关）
        double consumption = base_energy * (1.0 + mobility * 0.5);
        energy -= consumption;

        // 温度影响
        double temp_diff = abs(env.temperature - preferred_temp);
        if (temp_diff > temp_tolerance) {
            lose_energy(temp_diff * 0.1);
        }

        // 疾病影响
        disease_effects();

        // 处理冬眠
        handle_hibernation(env);

        // 处理饥饿
        handle_hunger();
    }

    // 将handle_hibernation改为虚函数
    virtual void handle_hibernation(Environment& env) {
        if (env.temperature < 5 && energy > 20 && !is_aquatic) {
            is_hibernating = true;
            lose_energy(0.1); // 冬眠时消耗少量能量
        }
        else if (env.temperature > 10 || energy < 10) {
            is_hibernating = false;
        }
    }

    void handle_hunger() {
        if (energy < max_age / 10.0) { // 能量过低
            days_without_food++;
            if (days_without_food > 5) {
                lose_energy(energy * 0.2); // 饥饿致死
            }
        }
        else {
            days_without_food = 0;
        }
    }

    bool is_dead() const { return energy <= 0 || age >= max_age; }
    void gain_energy(double amount) {
        energy += amount;
        // 能量上限
        energy = min(energy, static_cast<double>(max_age * 2));
    }
    void lose_energy(double amount) {
        energy -= amount;
        if (energy < 0) energy = 0;
    }

    // 位置和能量访问
    int getX() const { return x; }
    int getY() const { return y; }
    double getEnergy() const { return energy; }
    bool getIsAquatic() const { return is_aquatic; }
    bool isHibernating() const { return is_hibernating; }
    double getMobility() const { return mobility; }  // 添加getMobility
    void setPosition(int new_x, int new_y) { x = new_x; y = new_y; }  // 添加setPosition

    // 繁殖机会检查
    bool can_reproduce() const {
        return energy > reproduction_threshold &&
            (rand() / (double)RAND_MAX) < reproduction_chance &&
            !is_hibernating && !has_disease && age > max_age / 4;
    }

    // 环境适应度
    virtual double environment_fitness(Environment& env, Terrain& terrain) {
        // 温度影响
        double temp_diff = abs(env.temperature - preferred_temp);
        double temp_fitness = 1.0 - min(1.0, temp_diff / temp_tolerance);

        // 白天时长影响 (夜行性/昼行性)
        double daylight_fitness = (preferred_temp > 30) ?
            min(1.0, env.daylight_hours / 12.0) :  // 喜热生物偏好白天
            min(1.0, (24 - env.daylight_hours) / 12.0); // 喜冷生物偏好夜晚

        // 地形适应度
        double terrain_fitness = 0.0;
        switch (terrain.type) {
        case PLAIN: terrain_fitness = 0.8; break;
        case FOREST: terrain_fitness = 0.9; break;
        case MOUNTAIN: terrain_fitness = 0.4; break;
        case DESERT: terrain_fitness = 0.3; break;
        case WATER: terrain_fitness = is_aquatic ? 1.0 : 0.1; break;
        case MARSH: terrain_fitness = 0.7; break;
        case VOLCANIC: terrain_fitness = 0.2; break;
        case SNOW: terrain_fitness = 0.5; break;
        case GRASSLAND: terrain_fitness = 0.85; break;
        case JUNGLE: terrain_fitness = 0.95; break;
        case TUNDRA: terrain_fitness = 0.4; break;
        case BEACH: terrain_fitness = is_aquatic ? 0.7 : 0.6; break;
        case FLOODED: terrain_fitness = is_aquatic ? 0.9 : 0.3; break;
        }

        // 污染影响
        double pollution_fitness = 1.0 - max(env.pollution, terrain.pollution_level);

        // 疾病影响
        double disease_fitness = 1.0 - terrain.disease_level;

        // 积水影响
        double flood_fitness = 1.0 - terrain.water_accumulation * (1.0 - flood_resistance);

        // 干旱影响
        double drought_fitness = 1.0 - terrain.drought_level * (1.0 - drought_resistance);

        // 综合适应度
        return (temp_fitness * 0.3 +
            daylight_fitness * 0.15 +
            terrain_fitness * 0.15 +
            pollution_fitness * 0.1 +
            disease_fitness * 0.1 +
            flood_fitness * 0.1 +
            drought_fitness * 0.1);
    }

    // 寻找附近同类
    vector<Organism*> find_nearby_species(vector<Organism*>& organisms, int range) {
        vector<Organism*> nearby;
        for (Organism* org : organisms) {
            if (typeid(*org) == typeid(*this)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= range && dy <= range && !org->is_dead()) {
                    nearby.push_back(org);
                }
            }
        }
        return nearby;
    }
};

// 植物类
class Plant : public Organism {
protected:
    double growth_rate; // 生长速率
    double water_need;  // 水分需求
    int growth_stage;   // 0=种子, 1=幼苗, 2=成熟, 3=开花, 4=结果
    int days_to_mature; // 成熟所需天数
    double seed_spread_range; // 种子传播范围
    double flood_tolerance; // 抗洪能力
    double drought_tolerance; // 抗旱能力

public:
    Plant(int x, int y, double energy = 10.0) : Organism(x, y, energy) {
        max_age = 50;
        reproduction_threshold = 15.0;
        reproduction_chance = 0.4;
        growth_rate = 0.2;
        water_need = 0.6;
        preferred_temp = 22.0;
        temp_tolerance = 20.0;
        growth_stage = 0;
        days_to_mature = 20;
        seed_spread_range = 5.0;
        disease_resistance = 30;
        flood_resistance = 0.3;
        drought_resistance = 0.6;
        flood_tolerance = 0.4;
        drought_tolerance = 0.7;
        base_energy = 0.05;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        // 植物不移动，但种子可以传播
        if (growth_stage >= 3) { // 开花或结果阶段可以传播种子
            if (rand() % 100 < 5) { // 5%几率传播种子
                int new_x = x + rand() % static_cast<int>(seed_spread_range * 2) - static_cast<int>(seed_spread_range);
                int new_y = y + rand() % static_cast<int>(seed_spread_range * 2) - static_cast<int>(seed_spread_range);

                // 边界检查
                new_x = max(0, min(static_cast<int>(terrain[0].size()) - 1, new_x));
                new_y = max(0, min(static_cast<int>(terrain.size()) - 1, new_y));

                // 创建新植物（种子）
                if (canInhabit(terrain[new_y][new_x].type)) {
                    Organism* new_plant = new Plant(new_x, new_y, 5.0);
                    // 添加到全局生物列表
                    // 需要World类的上下文，暂时注释
                    // organisms.push_back(new_plant);
                }
            }
        }
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(0.5);
            return;
        }

        // 植物生长阶段推进
        if (growth_stage < 4 && age > days_to_mature * (growth_stage + 1) / 4) {
            growth_stage++;
            if (growth_stage == 2) { // 成熟期
                reproduction_chance += 0.1;
            }
        }

        // 植物通过光合作用获取能量
        double light_factor = min(1.0, env.daylight_hours / 12.0);
        double fertility_factor = terrain[y][x].fertility;
        double water_factor = min(1.0, terrain[y][x].water_level / water_need);

        // 降雨影响
        water_factor = min(1.0, water_factor + env.rainfall / 100.0);

        // 干旱影响
        if (terrain[y][x].drought_level > 0.5) {
            water_factor *= (1.0 - terrain[y][x].drought_level);
        }

        double growth = growth_rate * light_factor *
            water_factor * fertility_factor *
            environment_fitness(env, terrain[y][x]);

        // 生长阶段影响生长速度
        growth *= (1.0 + growth_stage * 0.2);

        // 积水影响
        if (terrain[y][x].water_accumulation > flood_tolerance) {
            growth *= 0.5; // 积水过多会抑制生长
        }

        gain_energy(growth);
    }

    Organism* reproduce() override {
        if (can_reproduce() && growth_stage >= 2) { // 只有成熟期以上植物可以繁殖
            energy /= 2;
            Plant* child = new Plant(x + rand() % 5 - 2, y + rand() % 5 - 2);
            // 遗传变异
            child->growth_rate = max(0.1, min(0.3, growth_rate + (rand() % 11 - 5) * 0.01));
            child->drought_resistance = max(0.3, min(0.8, drought_resistance + (rand() % 11 - 5) * 0.02));
            return child;
        }
        return nullptr;
    }

    string getSymbol() const override {
        if (growth_stage == 0) return "s"; // 种子
        if (growth_stage == 1) return "p"; // 幼苗
        return "P"; // 成熟植物
    }
    string getName() const override {
        switch (growth_stage) {
        case 0: return "种子";
        case 1: return "幼苗";
        case 2: return "植物";
        case 3: return "开花植物";
        default: return "结果植物";
        }
    }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != FLOODED;
    }

    void disease_effects() override {
        if (!has_disease) return;

        // 植物疾病影响更大
        lose_energy(1.0);
        growth_rate *= 0.5;

        // 较高概率死亡
        if (rand() % 100 < 10) {
            lose_energy(energy);
        }

        // 较低概率康复
        if (rand() % 100 < disease_resistance / 5) {
            has_disease = false;
        }
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 干旱天气影响
        if (env.weather == DROUGHT && terrain.drought_level > drought_tolerance) {
            lose_energy(0.8);
        }

        // 暴雨天气影响
        if (env.weather == STORMY && terrain.water_accumulation > flood_tolerance) {
            lose_energy(1.0);
        }
    }
};

// 树木类 - 森林植物
class Tree : public Plant {
public:
    Tree(int x, int y, double energy = 20.0) : Plant(x, y, energy) {
        max_age = 200;
        reproduction_threshold = 30.0;
        reproduction_chance = 0.3;
        growth_rate = 0.15;
        water_need = 0.7;
        preferred_temp = 20.0;
        temp_tolerance = 15.0;
        days_to_mature = 50;
        seed_spread_range = 10.0;
        flood_resistance = 0.2;
        drought_resistance = 0.8;
        base_energy = 0.03;
    }

    Organism* reproduce() override {
        if (can_reproduce() && growth_stage >= 3) { // 只有开花期以上树木可以繁殖
            energy /= 2;
            Tree* child = new Tree(x + rand() % 3 - 1, y + rand() % 3 - 1);
            // 遗传变异
            child->growth_rate = max(0.1, min(0.2, growth_rate + (rand() % 11 - 5) * 0.005));
            return child;
        }
        return nullptr;
    }

    string getSymbol() const override {
        if (growth_stage < 2) return "t";
        return "T";
    }
    string getName() const override {
        if (growth_stage < 2) return "小树苗";
        return "树木";
    }

    bool canInhabit(TerrainType type) const override {
        return type == FOREST || type == PLAIN || type == JUNGLE;
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴风雨可能吹倒树木
        if (env.weather == STORMY && growth_stage < 3 && rand() % 100 < 10) {
            lose_energy(energy * 0.5);
        }
    }
};

// 水生植物类
class AquaticPlant : public Plant {
public:
    AquaticPlant(int x, int y, double energy = 8.0) : Plant(x, y, energy) {
        max_age = 40;
        reproduction_threshold = 12.0;
        reproduction_chance = 0.5;
        growth_rate = 0.25;
        water_need = 1.0;
        preferred_temp = 18.0;
        temp_tolerance = 10.0;
        is_aquatic = true;
        days_to_mature = 10;
        seed_spread_range = 3.0;
        flood_resistance = 0.9;
        drought_resistance = 0.1;
        base_energy = 0.04;
    }

    Organism* reproduce() override {
        if (can_reproduce() && growth_stage >= 2) {
            energy /= 2;
            AquaticPlant* child = new AquaticPlant(x + rand() % 3 - 1, y + rand() % 3 - 1);
            // 遗传变异
            child->growth_rate = max(0.2, min(0.3, growth_rate + (rand() % 11 - 5) * 0.01));
            return child;
        }
        return nullptr;
    }

    string getSymbol() const override {
        if (growth_stage < 2) return "a";
        return "A";
    }
    string getName() const override {
        if (growth_stage < 2) return "幼年水生植物";
        return "水生植物";
    }

    bool canInhabit(TerrainType type) const override {
        return type == WATER || type == MARSH || type == FLOODED;
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 干旱天气对水生植物影响很大
        if (env.weather == DROUGHT) {
            lose_energy(1.0);
        }
    }
};

// 动物移动函数增强
void animal_move(Organism* org, vector<vector<Terrain>>& terrain, Environment& env, int base_range) {
    if (org->isHibernating()) return; // 冬眠期间不移动

    // 积雪影响移动能力
    double snow_factor = 1.0 - min(0.5, terrain[org->getY()][org->getX()].snow_depth * 0.7);

    // 积水影响移动能力
    double flood_factor = 1.0;
    if (!org->getIsAquatic() && terrain[org->getY()][org->getX()].water_accumulation > 0.3) {
        flood_factor = 0.6;
    }

    int move_range = static_cast<int>(org->getMobility() * base_range * snow_factor * flood_factor);
    int new_x = org->getX() + rand() % (move_range * 2 + 1) - move_range;
    int new_y = org->getY() + rand() % (move_range * 2 + 1) - move_range;

    // 边界检查
    new_x = max(0, min(static_cast<int>(terrain[0].size()) - 1, new_x));
    new_y = max(0, min(static_cast<int>(terrain.size()) - 1, new_y));

    // 检查新位置是否适合栖息
    if (org->canInhabit(terrain[new_y][new_x].type)) {
        org->setPosition(new_x, new_y);
        org->save_previous_state("移动");
    }

    org->lose_energy(0.2);
}

// 昆虫类
class Insect : public Organism {
protected:
    bool is_flying;
    bool is_nocturnal; // 是否夜行性

public:
    Insect(int x, int y, double energy = 5.0) : Organism(x, y, energy) {
        max_age = 30;
        reproduction_threshold = 8.0;
        reproduction_chance = 0.5;
        mobility = 1.5;
        preferred_temp = 28.0;
        temp_tolerance = 25.0;
        is_flying = false;
        is_nocturnal = (rand() % 2 == 0);
        disease_resistance = 40;
        flood_resistance = 0.1;
        drought_resistance = 0.8;
        base_energy = 0.08;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        // 如果是夜行性昆虫，白天活动减少
        if (is_nocturnal && env.daylight_hours > 12) {
            if (rand() % 100 < 70) return; // 70%几率不活动
        }

        // 雨天使昆虫活动减少
        if (env.weather == RAINY || env.weather == STORMY) {
            if (rand() % 100 < 60) return;
        }

        // 小范围移动
        int move_range = is_flying ? 5 : 3;
        animal_move(this, terrain, env, move_range);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(0.5);
            return;
        }

        // 干旱天气影响食物获取
        if (env.weather == DROUGHT && terrain[y][x].drought_level > 0.5) {
            lose_energy(0.3);
            return;
        }

        // 寻找附近的植物或腐肉
        for (Organism* org : organisms) {
            if (dynamic_cast<Plant*>(org) && !dynamic_cast<Tree*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 1 && dy <= 1) {
                    // 吃植物
                    gain_energy(org->getEnergy() * 0.7);
                    org->lose_energy(org->getEnergy());
                    save_previous_state("进食");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 3);
            if (nearby.size() > 1) { // 至少有一个配偶
                energy /= 2;
                Insect* child = new Insect(x + rand() % 2 - 1, y + rand() % 2 - 1);
                // 遗传变异
                child->mobility = max(1.0, min(2.0, mobility + (rand() % 11 - 5) * 0.05));
                child->disease_resistance = max(30, min(50, disease_resistance + rand() % 11 - 5));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "I"; }
    string getName() const override { return "昆虫"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != FLOODED;
    }

    double environment_fitness(Environment& env, Terrain& terrain) override {
        double fitness = Organism::environment_fitness(env, terrain);

        // 夜行性昆虫在夜晚更活跃
        if (is_nocturnal && env.daylight_hours < 12) {
            fitness *= 1.2;
        }
        return min(1.0, fitness);
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴雨可能冲走昆虫
        if (env.weather == STORMY && !is_flying && rand() % 100 < 30) {
            lose_energy(1.0);
        }
    }
};

// 飞行昆虫类
class FlyingInsect : public Insect {
public:
    FlyingInsect(int x, int y, double energy = 4.0) : Insect(x, y, energy) {
        max_age = 20;
        reproduction_threshold = 6.0;
        reproduction_chance = 0.6;
        mobility = 2.0;
        preferred_temp = 30.0;
        is_flying = true;
        territory_size = 5;
        flood_resistance = 0.8; // 飞行昆虫不怕积水
        base_energy = 0.1;
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(0.5);
            return;
        }

        // 飞行昆虫可以吃花蜜和小型昆虫
        for (Organism* org : organisms) {
            if (dynamic_cast<Plant*>(org) || dynamic_cast<Insect*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 2 && dy <= 2) {
                    // 吃植物或昆虫
                    gain_energy(org->getEnergy() * 0.5);
                    org->lose_energy(org->getEnergy() * 0.8);
                    save_previous_state("进食");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 5);
            if (nearby.size() > 1) {
                energy *= 0.5;
                FlyingInsect* child = new FlyingInsect(x + rand() % 3 - 1, y + rand() % 3 - 1);
                // 遗传变异
                child->mobility = max(1.8, min(2.5, mobility + (rand() % 11 - 5) * 0.05));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "F"; }
    string getName() const override { return "飞行昆虫"; }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴雨影响飞行
        if (env.weather == STORMY && rand() % 100 < 40) {
            lose_energy(0.5);
        }
    }
};

// 食草动物类
class Herbivore : public Organism {
protected:
    bool migrated; // 是否已迁徙

public:
    Herbivore(int x, int y, double energy = 20.0) : Organism(x, y, energy) {
        max_age = 70;
        reproduction_threshold = 30.0;
        mobility = 1.2;
        preferred_temp = 22.0;
        temp_tolerance = 25.0;
        territory_size = 10;
        migrated = false;
        disease_resistance = 60;
        flood_resistance = 0.3;
        drought_resistance = 0.7;
        base_energy = 0.15;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        if (isHibernating()) return;

        // 季节性迁徙
        if (!migrated && (env.season_progress > 0.7 || env.season_progress < 0.3)) {
            int move_range = static_cast<int>(mobility * 50); // 长距离迁徙
            int new_x = x + rand() % (move_range * 2 + 1) - move_range;
            int new_y = y + rand() % (move_range * 2 + 1) - move_range;

            // 边界检查
            new_x = max(0, min(static_cast<int>(terrain[0].size()) - 1, new_x));
            new_y = max(0, min(static_cast<int>(terrain.size()) - 1, new_y));

            if (canInhabit(terrain[new_y][new_x].type)) {
                x = new_x;
                y = new_y;
                migrated = true;
                save_previous_state("迁徙");
                return;
            }
        }

        animal_move(this, terrain, env, 3);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(1.0);
            return;
        }

        // 干旱天气减少食物供应
        double food_availability = 1.0;
        if (env.weather == DROUGHT) {
            food_availability = 0.7;
        }

        // 寻找附近的植物
        vector<Organism*> nearby_plants;
        for (Organism* org : organisms) {
            if (dynamic_cast<Plant*>(org) && !dynamic_cast<Tree*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 2 && dy <= 2) {
                    nearby_plants.push_back(org);
                }
            }
        }

        // 吃能量最高的植物
        if (!nearby_plants.empty()) {
            sort(nearby_plants.begin(), nearby_plants.end(),
                [](Organism* a, Organism* b) {
                    return a->getEnergy() > b->getEnergy();
                });

            Organism* target = nearby_plants[0];
            gain_energy(target->getEnergy() * 0.6 * food_availability);
            target->lose_energy(target->getEnergy());
            save_previous_state("进食");
            return;
        }

        // 食草动物需要消耗更多能量在寒冷环境中
        if (env.temperature < 10) lose_energy(1.0);
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 5);
            if (nearby.size() > 1) {
                energy *= 0.4;
                migrated = false; // 重置迁徙状态
                Herbivore* child = new Herbivore(x + rand() % 3 - 1, y + rand() % 3 - 1);
                // 遗传变异
                child->max_age = max(50, min(80, max_age + rand() % 11 - 5));
                child->reproduction_threshold = max(25.0, min(35.0, reproduction_threshold + (rand() % 11 - 5)));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "H"; }
    string getName() const override { return "食草动物"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != MOUNTAIN && type != FLOODED;
    }

    void seasonal_effect(Environment& env) override {
        // 秋季增加繁殖几率
        if (fmod(env.season_progress, 1.0) > 0.6 && fmod(env.season_progress, 1.0) < 0.9) {
            reproduction_chance = min(0.6, reproduction_chance * 1.2);
        }
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴风雨影响食草动物
        if (env.weather == STORMY) {
            lose_energy(1.0);
        }
    }
};

// 鱼类
class Fish : public Organism {
public:
    Fish(int x, int y, double energy = 15.0) : Organism(x, y, energy) {
        max_age = 40;
        reproduction_threshold = 20.0;
        reproduction_chance = 0.35;
        mobility = 1.2;
        preferred_temp = 18.0;
        temp_tolerance = 10.0;
        is_aquatic = true;
        disease_resistance = 45;
        flood_resistance = 0.9;
        drought_resistance = 0.1;
        base_energy = 0.12;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        // 在水中移动
        int move_range = static_cast<int>(mobility * 3);
        int new_x = x + rand() % (move_range * 2 + 1) - move_range;
        int new_y = y + rand() % (move_range * 2 + 1) - move_range;

        // 边界检查
        new_x = max(0, min(static_cast<int>(terrain[0].size()) - 1, new_x));
        new_y = max(0, min(static_cast<int>(terrain.size()) - 1, new_y));

        // 只能在水中移动
        if (terrain[new_y][new_x].type == WATER || terrain[new_y][new_x].type == FLOODED) {
            x = new_x;
            y = new_y;
            save_previous_state("移动");
        }

        lose_energy(0.3);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 只能在水域进食
        if (terrain[y][x].type != WATER && terrain[y][x].type != FLOODED) {
            lose_energy(1.0);
            return;
        }

        // 寻找附近的水生植物或小型水生生物
        for (Organism* org : organisms) {
            if ((dynamic_cast<AquaticPlant*>(org) || dynamic_cast<Insect*>(org)) &&
                org->getIsAquatic()) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 2 && dy <= 2) {
                    // 进食
                    gain_energy(org->getEnergy() * 0.6);
                    org->lose_energy(org->getEnergy());
                    save_previous_state("进食");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 3);
            if (nearby.size() > 1) {
                energy *= 0.5;
                Fish* child = new Fish(x + rand() % 2 - 1, y + rand() % 2 - 1);
                // 遗传变异
                child->reproduction_chance = max(0.3, min(0.4, reproduction_chance + (rand() % 11 - 5) * 0.01));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "~"; }
    string getName() const override { return "鱼类"; }

    bool canInhabit(TerrainType type) const override {
        return type == WATER || type == FLOODED;
    }

    void seasonal_effect(Environment& env) override {
        // 水温变化影响鱼类
        double temp_diff = abs(env.temperature - preferred_temp);
        if (temp_diff > 5) {
            reproduction_chance *= 0.8;
        }
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 干旱对鱼类是灾难性的
        if (env.weather == DROUGHT) {
            lose_energy(2.0);
        }
        // 降雨有利鱼类
        else if (env.weather == RAINY) {
            gain_energy(0.2);
        }
    }
};

// 鸟类
class Bird : public Organism {
public:
    Bird(int x, int y, double energy = 25.0) : Organism(x, y, energy) {
        max_age = 50;
        reproduction_threshold = 30.0;
        reproduction_chance = 0.3;
        mobility = 2.5;
        preferred_temp = 22.0;
        temp_tolerance = 20.0;
        territory_size = 20;
        disease_resistance = 50;
        flood_resistance = 0.8; // 鸟类不怕洪水
        drought_resistance = 0.6;
        base_energy = 0.18;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        // 鸟类可以长距离移动
        int move_range = static_cast<int>(mobility * 8);
        int new_x = x + rand() % (move_range * 2 + 1) - move_range;
        int new_y = y + rand() % (move_range * 2 + 1) - move_range;

        // 边界检查
        new_x = max(0, min(static_cast<int>(terrain[0].size()) - 1, new_x));
        new_y = max(0, min(static_cast<int>(terrain.size()) - 1, new_y));

        // 鸟类可以跨越大部分地形
        if (terrain[new_y][new_x].type != WATER) {
            x = new_x;
            y = new_y;
            save_previous_state("飞行");
        }

        lose_energy(0.8);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 寻找附近的昆虫、鱼类或小型动物
        for (Organism* org : organisms) {
            if (dynamic_cast<Insect*>(org) || dynamic_cast<Fish*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 3 && dy <= 3) {
                    // 捕食
                    gain_energy(org->getEnergy() * 0.7);
                    org->lose_energy(org->getEnergy());
                    save_previous_state("进食");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 10);
            if (nearby.size() > 1) {
                energy *= 0.5;
                Bird* child = new Bird(x + rand() % 5 - 2, y + rand() % 5 - 2);
                // 遗传变异
                child->mobility = max(2.0, min(3.0, mobility + (rand() % 11 - 5) * 0.1));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "B"; }  // 改为B避免与顶级掠食者冲突
    string getName() const override { return "鸟类"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC;
    }

    void seasonal_effect(Environment& env) override {
        // 春季增加繁殖几率
        if (fmod(env.season_progress, 1.0) > 0.2 && fmod(env.season_progress, 1.0) < 0.5) {
            reproduction_chance = min(0.5, reproduction_chance * 1.5);
        }
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴风雨影响鸟类飞行
        if (env.weather == STORMY) {
            lose_energy(1.0);
        }
    }
};

// 分解者类（分解死亡生物）
class Decomposer : public Organism {
public:
    Decomposer(int x, int y, double energy = 3.0) : Organism(x, y, energy) {
        max_age = 40;
        reproduction_threshold = 6.0;
        reproduction_chance = 0.45;
        mobility = 0.5;
        preferred_temp = 25.0;
        disease_resistance = 80; // 分解者抵抗力强
        flood_resistance = 0.4;
        drought_resistance = 0.7;
        base_energy = 0.06;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        // 缓慢移动
        if (rand() % 5 == 0) {
            animal_move(this, terrain, env, 2);
        }
        lose_energy(0.1);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(0.3);
            return;
        }

        // 寻找附近死亡的生物
        for (Organism* org : organisms) {
            if (org->is_dead() && !dynamic_cast<Decomposer*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 1 && dy <= 1) {
                    // 分解死亡生物
                    gain_energy(org->getEnergy() * 0.8);
                    org->lose_energy(org->getEnergy());

                    // 增加土壤肥力
                    terrain[y][x].fertility = min(1.0, terrain[y][x].fertility + 0.01);
                    save_previous_state("分解");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            energy /= 2;
            Decomposer* child = new Decomposer(x + rand() % 2 - 1, y + rand() % 2 - 1);
            // 遗传变异
            child->disease_resistance = max(70, min(90, disease_resistance + rand() % 11 - 5));
            save_previous_state("繁殖");
            return child;
        }
        return nullptr;
    }

    string getSymbol() const override { return "D"; }
    string getName() const override { return "分解者"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != FLOODED;
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 雨天有利于分解者
        if (env.weather == RAINY) {
            gain_energy(0.1);
        }
    }
};

// 杂食动物类（既吃植物又吃小动物）
class Omnivore : public Organism {
public:
    Omnivore(int x, int y, double energy = 25.0) : Organism(x, y, energy) {
        max_age = 65;
        reproduction_threshold = 35.0;
        reproduction_chance = 0.28;
        mobility = 1.3;
        preferred_temp = 24.0;
        territory_size = 8;
        disease_resistance = 55;
        flood_resistance = 0.4;
        drought_resistance = 0.6;
        base_energy = 0.16;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        animal_move(this, terrain, env, 3);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(1.0);
            return;
        }

        // 干旱天气减少食物供应
        double food_availability = 1.0;
        if (env.weather == DROUGHT) {
            food_availability = 0.8;
        }

        // 寻找附近的植物或小动物
        vector<Organism*> potential_food;
        for (Organism* org : organisms) {
            if (dynamic_cast<Plant*>(org) ||
                dynamic_cast<Insect*>(org) ||
                dynamic_cast<Fish*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 2 && dy <= 2) {
                    potential_food.push_back(org);
                }
            }
        }

        // 选择最近的食物
        if (!potential_food.empty()) {
            sort(potential_food.begin(), potential_food.end(),
                [this](Organism* a, Organism* b) {
                    int dx1 = abs(x - a->getX());
                    int dy1 = abs(y - a->getY());
                    int dx2 = abs(x - b->getX());
                    int dy2 = abs(y - b->getY());
                    return (dx1 + dy1) < (dx2 + dy2);
                });

            Organism* target = potential_food[0];
            gain_energy(target->getEnergy() * 0.5 * food_availability);
            target->lose_energy(target->getEnergy());
            save_previous_state("进食");
            return;
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 5);
            if (nearby.size() > 1) {
                energy *= 0.4;
                Omnivore* child = new Omnivore(x + rand() % 3 - 1, y + rand() % 3 - 1);
                // 遗传变异
                child->reproduction_threshold = max(30.0, min(40.0, reproduction_threshold + (rand() % 11 - 5)));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "O"; }
    string getName() const override { return "杂食动物"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != FLOODED;
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴风雨影响
        if (env.weather == STORMY) {
            lose_energy(1.0);
        }
    }
};

// 食肉动物类
class Carnivore : public Organism {
protected:
    int hunting_skill; // 狩猎技能 (0-100)

public:
    Carnivore(int x, int y, double energy = 30.0) : Organism(x, y, energy) {
        max_age = 60;
        reproduction_threshold = 40.0;
        reproduction_chance = 0.25;
        mobility = 1.8;
        preferred_temp = 20.0;
        territory_size = 15;
        hunting_skill = 50 + rand() % 40; // 50-90
        disease_resistance = 65;
        flood_resistance = 0.3;
        drought_resistance = 0.5;
        base_energy = 0.2;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        animal_move(this, terrain, env, 4);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(1.5);
            return;
        }

        // 寻找附近的食草动物或杂食动物
        vector<Organism*> prey_list;
        for (Organism* org : organisms) {
            if (dynamic_cast<Herbivore*>(org) ||
                dynamic_cast<Omnivore*>(org) ||
                dynamic_cast<Bird*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 3 && dy <= 3) {
                    prey_list.push_back(org);
                }
            }
        }

        // 选择猎物
        if (!prey_list.empty()) {
            // 选择最弱的猎物
            sort(prey_list.begin(), prey_list.end(),
                [](Organism* a, Organism* b) {
                    return a->getEnergy() < b->getEnergy();
                });

            Organism* target = prey_list[0];

            // 狩猎成功概率取决于狩猎技能
            if (rand() % 100 < hunting_skill) {
                gain_energy(target->getEnergy() * 0.7);
                target->lose_energy(target->getEnergy());
                save_previous_state("捕猎成功");
            }
            else {
                // 狩猎失败也消耗能量
                lose_energy(2.0);
                save_previous_state("捕猎失败");
            }
            return;
        }

        // 食肉动物在炎热环境中消耗更多能量
        if (env.temperature > 35) lose_energy(2.0);
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 8);
            if (nearby.size() > 1) {
                energy *= 0.4;
                Carnivore* child = new Carnivore(x + rand() % 2 - 1, y + rand() % 2 - 1);
                // 后代继承部分狩猎技能
                child->hunting_skill = max(20, min(100, hunting_skill - 10 + rand() % 20));
                // 遗传变异
                child->mobility = max(1.5, min(2.2, mobility + (rand() % 11 - 5) * 0.05));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "C"; }
    string getName() const override { return "食肉动物"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != FLOODED;
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴风雨影响狩猎
        if (env.weather == STORMY) {
            lose_energy(1.0);
        }
    }
};

// 顶级掠食者类
class ApexPredator : public Organism {
public:
    ApexPredator(int x, int y, double energy = 50.0) : Organism(x, y, energy) {
        max_age = 80;
        reproduction_threshold = 60.0;
        reproduction_chance = 0.15;
        mobility = 2.0;
        preferred_temp = 18.0;
        territory_size = 50;
        disease_resistance = 70;
        flood_resistance = 0.5;
        drought_resistance = 0.7;
        base_energy = 0.25;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        animal_move(this, terrain, env, 5);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(2.0);
            return;
        }

        // 寻找附近的食肉动物或杂食动物
        for (Organism* org : organisms) {
            if (dynamic_cast<Carnivore*>(org) ||
                dynamic_cast<Omnivore*>(org) ||
                dynamic_cast<Herbivore*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 4 && dy <= 4) {
                    // 捕食
                    gain_energy(org->getEnergy() * 0.8);
                    org->lose_energy(org->getEnergy());
                    save_previous_state("捕食");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 15);
            if (nearby.size() > 1) {
                energy *= 0.3;
                ApexPredator* child = new ApexPredator(x + rand() % 3 - 1, y + rand() % 3 - 1);
                // 遗传变异
                child->max_age = max(70, min(90, max_age + rand() % 11 - 5));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "X"; } // 改为X避免与水生植物冲突
    string getName() const override { return "顶级掠食者"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != FLOODED;
    }

    void seasonal_effect(Environment& env) override {
        // 冬季减少活动
        if (fmod(env.season_progress, 1.0) > 0.7 || fmod(env.season_progress, 1.0) < 0.3) {
            mobility *= 0.8;
        }
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 暴风雨影响顶级掠食者
        if (env.weather == STORMY) {
            lose_energy(1.5);
        }
    }
};

// 寄生生物类
class Parasite : public Organism {
public:
    Parasite(int x, int y, double energy = 2.0) : Organism(x, y, energy) {
        max_age = 20;
        reproduction_threshold = 4.0;
        reproduction_chance = 0.6;
        mobility = 0.1;
        disease_resistance = 90; // 寄生虫抵抗力强
        flood_resistance = 0.7;
        drought_resistance = 0.3;
        base_energy = 0.03;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        // 寄生生物不主动移动，依附宿主移动
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 寄生在宿主身上获取能量
        for (Organism* org : organisms) {
            if (!dynamic_cast<Parasite*>(org) && !org->is_dead()) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx == 0 && dy == 0) {
                    // 从宿主获取能量
                    double energy_taken = min(0.1, org->getEnergy() * 0.05);
                    gain_energy(energy_taken);
                    org->lose_energy(energy_taken);

                    // 传播疾病
                    if (rand() % 100 < 20) {
                        org->contract_disease(env.disease);
                    }
                    save_previous_state("寄生");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            energy /= 2;
            Parasite* child = new Parasite(x, y);
            // 遗传变异
            child->reproduction_chance = max(0.5, min(0.7, reproduction_chance + (rand() % 11 - 5) * 0.01));
            save_previous_state("繁殖");
            return child;
        }
        return nullptr;
    }

    string getSymbol() const override { return "*"; }
    string getName() const override { return "寄生生物"; }

    bool canInhabit(TerrainType type) const override {
        return true; // 寄生生物可以存在于任何地形
    }
};

// 爬行动物
class Reptile : public Organism {
public:
    Reptile(int x, int y, double energy = 22.0) : Organism(x, y, energy) {
        max_age = 60;
        reproduction_threshold = 25.0;
        reproduction_chance = 0.25;
        mobility = 1.0;
        preferred_temp = 30.0;
        temp_tolerance = 15.0;
        territory_size = 5;
        disease_resistance = 55;
        flood_resistance = 0.4;
        drought_resistance = 0.8;
        base_energy = 0.14;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        animal_move(this, terrain, env, 3);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(1.0);
            return;
        }

        // 寻找附近的昆虫、小型哺乳动物或蛋
        for (Organism* org : organisms) {
            if (dynamic_cast<Insect*>(org) || dynamic_cast<Herbivore*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 2 && dy <= 2) {
                    // 捕食
                    gain_energy(org->getEnergy() * 0.6);
                    org->lose_energy(org->getEnergy());
                    save_previous_state("进食");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 4);
            if (nearby.size() > 1) {
                energy *= 0.5;
                Reptile* child = new Reptile(x + rand() % 2 - 1, y + rand() % 2 - 1);
                // 遗传变异
                child->preferred_temp = max(25.0, min(35.0, preferred_temp + (rand() % 11 - 5)));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "R"; }
    string getName() const override { return "爬行动物"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != SNOW && type != VOLCANIC && type != FLOODED;
    }

    void handle_hibernation(Environment& env) override {
        // 爬行动物更早开始冬眠
        if (env.temperature < 10) {
            is_hibernating = true;
            lose_energy(0.05);
        }
        else if (env.temperature > 15) {
            is_hibernating = false;
        }
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 雨天爬行动物更活跃
        if (env.weather == RAINY) {
            gain_energy(0.1);
        }
    }
};

// 两栖动物
class Amphibian : public Organism {
public:
    Amphibian(int x, int y, double energy = 18.0) : Organism(x, y, energy) {
        max_age = 45;
        reproduction_threshold = 22.0;
        reproduction_chance = 0.35;
        mobility = 1.0;
        preferred_temp = 25.0;
        temp_tolerance = 15.0;
        disease_resistance = 40;
        flood_resistance = 0.8;
        drought_resistance = 0.3;
        base_energy = 0.12;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        animal_move(this, terrain, env, 3);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 寻找附近的昆虫或小型水生生物
        for (Organism* org : organisms) {
            if (dynamic_cast<Insect*>(org) || dynamic_cast<Fish*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 2 && dy <= 2) {
                    // 捕食
                    gain_energy(org->getEnergy() * 0.7);
                    org->lose_energy(org->getEnergy());
                    save_previous_state("进食");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 4);
            if (nearby.size() > 1) {
                energy *= 0.5;
                Amphibian* child = new Amphibian(x + rand() % 2 - 1, y + rand() % 2 - 1);
                // 遗传变异
                child->flood_resistance = max(0.7, min(0.9, flood_resistance + (rand() % 11 - 5) * 0.01));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "M"; }
    string getName() const override { return "两栖动物"; }

    bool canInhabit(TerrainType type) const override {
        return type == WATER || type == MARSH || type == PLAIN || type == FOREST || type == FLOODED;
    }

    void seasonal_effect(Environment& env) override {
        // 雨季增加活动
        if (env.rainfall > 50) {
            mobility *= 1.2;
            reproduction_chance *= 1.3;
        }
    }

    void weather_effect(Environment& env, Terrain& terrain) override {
        // 雨天两栖动物更活跃
        if (env.weather == RAINY) {
            gain_energy(0.2);
            reproduction_chance = min(0.5, reproduction_chance * 1.2);
        }
        // 干旱对两栖动物不利
        else if (env.weather == DROUGHT) {
            lose_energy(1.0);
        }
    }
};

// 食腐动物
class Scavenger : public Organism {
public:
    Scavenger(int x, int y, double energy = 15.0) : Organism(x, y, energy) {
        max_age = 55;
        reproduction_threshold = 20.0;
        reproduction_chance = 0.4;
        mobility = 1.5;
        preferred_temp = 22.0;
        disease_resistance = 75; // 食腐动物抵抗力强
        flood_resistance = 0.5;
        drought_resistance = 0.6;
        base_energy = 0.15;
    }

    void move(vector<vector<Terrain>>& terrain, Environment& env) override {
        animal_move(this, terrain, env, 4);
    }

    void eat(Environment& env, vector<Organism*>& organisms, vector<vector<Terrain>>& terrain) override {
        // 检查地形是否适合
        if (!canInhabit(terrain[y][x].type)) {
            lose_energy(0.8);
            return;
        }

        // 寻找附近死亡的生物
        for (Organism* org : organisms) {
            if (org->is_dead() && !dynamic_cast<Decomposer*>(org)) {
                int dx = abs(x - org->getX());
                int dy = abs(y - org->getY());
                if (dx <= 3 && dy <= 3) {
                    // 吃腐肉
                    gain_energy(org->getEnergy() * 0.7);
                    org->lose_energy(org->getEnergy());

                    // 可能感染疾病
                    if (rand() % 100 < 20) {
                        contract_disease(env.disease);
                    }
                    save_previous_state("食腐");
                    return;
                }
            }
        }
    }

    Organism* reproduce() override {
        if (can_reproduce()) {
            // 检查附近是否有配偶
            vector<Organism*> nearby = find_nearby_species(organisms, 5);
            if (nearby.size() > 1) {
                energy *= 0.5;
                Scavenger* child = new Scavenger(x + rand() % 3 - 1, y + rand() % 3 - 1);
                // 遗传变异
                child->disease_resistance = max(65, min(85, disease_resistance + rand() % 11 - 5));
                save_previous_state("繁殖");
                return child;
            }
        }
        return nullptr;
    }

    string getSymbol() const override { return "S"; }
    string getName() const override { return "食腐动物"; }

    bool canInhabit(TerrainType type) const override {
        return type != WATER && type != VOLCANIC && type != FLOODED;
    }
};

// 世界模拟器类
class World {
private:
    const int width = 1000;  // 世界宽度
    const int height = 1000; // 世界高度
    Environment env;
    vector<Organism*> organisms;
    vector<vector<Terrain>> terrain;
    int day;
    int season; // 0-春,1-夏,2-秋,3-冬
    int viewport_x, viewport_y; // 视口位置
    int viewport_width, viewport_height; // 视口尺寸
    const int max_days = 730; // 最大模拟天数（两年）
    int selected_x, selected_y; // 选中的生物位置
    bool show_history; // 是否显示历史状态

    // 禁止复制和赋值
    World(const World&) = delete;
    World& operator=(const World&) = delete;

    // 平滑地图
    void smooth_map(vector<vector<double>>& map, int iterations = 1) {
        int h = map.size();
        int w = map[0].size();
        vector<vector<double>> new_map(h, vector<double>(w));

        for (int iter = 0; iter < iterations; iter++) {
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    double sum = 0.0;
                    int count = 0;
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int nx = x + dx;
                            int ny = y + dy;
                            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                                sum += map[ny][nx];
                                count++;
                            }
                        }
                    }
                    new_map[y][x] = sum / count;
                }
            }
            map = new_map;
        }
    }

    // 生成地形
    void generate_terrain() {
        terrain.resize(height, vector<Terrain>(width));

        // 使用分形噪声生成高度图
        vector<vector<double>> height_map(height, vector<double>(width, 0.0));
        generate_fractal_noise(height_map, 8, 0.5); // 增加octaves，降低persistence

        // 平滑高度图
        smooth_map(height_map, 2);

        // 生成湿度图
        vector<vector<double>> moisture_map(height, vector<double>(width, 0.0));
        generate_fractal_noise(moisture_map, 8, 0.5);

        // 平滑湿度图
        smooth_map(moisture_map, 2);

        // 根据高度和湿度设置地形
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double h = height_map[y][x];
                double m = moisture_map[y][x];

                // 计算纬度因子（0-1，0为赤道，1为两极）
                double lat_factor = 2.0 * abs(y - height / 2.0) / height;

                terrain[y][x].height = h;
                terrain[y][x].original_height = h; // 保存原始高度
                terrain[y][x].water_level = m;

                if (h < 0.2) {
                    terrain[y][x].type = WATER;
                    terrain[y][x].water_level = 1.0;
                    terrain[y][x].fertility = 0.3;
                }
                else if (h < 0.25) {
                    terrain[y][x].type = BEACH;
                    terrain[y][x].water_level = 0.9;
                    terrain[y][x].fertility = 0.5;
                }
                else if (h < 0.3) {
                    terrain[y][x].type = MARSH;
                    terrain[y][x].water_level = 0.8;
                    terrain[y][x].fertility = 0.7;
                }
                else if (h < 0.5) {
                    if (m > 0.7) {
                        if (lat_factor < 0.3) {
                            terrain[y][x].type = JUNGLE;
                        }
                        else {
                            terrain[y][x].type = FOREST;
                        }
                        terrain[y][x].fertility = 0.9;
                    }
                    else if (m > 0.4) {
                        terrain[y][x].type = PLAIN;
                        terrain[y][x].fertility = 0.7;
                    }
                    else {
                        terrain[y][x].type = GRASSLAND;
                        terrain[y][x].fertility = 0.8;
                    }
                    terrain[y][x].water_level = m * 0.5;
                }
                else if (h < 0.7) {
                    if (m < 0.3) {
                        terrain[y][x].type = DESERT;
                        terrain[y][x].fertility = 0.2;
                    }
                    else if (m < 0.6) {
                        terrain[y][x].type = GRASSLAND;
                        terrain[y][x].fertility = 0.7;
                    }
                    else {
                        terrain[y][x].type = PLAIN;
                        terrain[y][x].fertility = 0.6;
                    }
                    terrain[y][x].water_level = m * 0.3;
                }
                else if (h < 0.9) {
                    if (lat_factor > 0.6) {
                        terrain[y][x].type = TUNDRA;
                        terrain[y][x].fertility = 0.4;
                    }
                    else {
                        terrain[y][x].type = MOUNTAIN;
                        terrain[y][x].fertility = 0.4;
                    }
                    terrain[y][x].water_level = m * 0.2;
                }
                else {
                    if (rand() % 100 < 10) {
                        terrain[y][x].type = VOLCANIC;
                        terrain[y][x].fertility = 0.1;
                    }
                    else {
                        terrain[y][x].type = MOUNTAIN;
                        terrain[y][x].fertility = 0.3;
                    }
                    terrain[y][x].water_level = m * 0.1;
                }

                // 添加雪地（基于高度和纬度）
                if (h > 0.6 && lat_factor > 0.7) {
                    terrain[y][x].type = SNOW;
                }
            }
        }
    }

    // 生成分形噪声
    void generate_fractal_noise(vector<vector<double>>& map, int octaves, double persistence) {
        int w = map[0].size();
        int h = map.size();

        // 基础噪声
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                map[y][x] = static_cast<double>(rand()) / RAND_MAX;
            }
        }

        // 多次叠加噪声
        double amplitude = 1.0;
        double max_amplitude = 0.0;
        vector<vector<double>> temp(h, vector<double>(w, 0.0));

        for (int octave = 0; octave < octaves; octave++) {
            int scale = 1 << octave;
            double freq = 1.0 / scale;

            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    int sample_x = (x * scale) % w;
                    int sample_y = (y * scale) % h;

                    temp[y][x] += map[sample_y][sample_x] * amplitude;
                }
            }

            max_amplitude += amplitude;
            amplitude *= persistence;
        }

        // 归一化
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                map[y][x] = temp[y][x] / max_amplitude;
            }
        }
    }

    // 季节变化影响
    void update_season() {
        // 两年共730天，每季约182.5天
        env.season_progress = fmod(day / 182.5, 4.0);
        season = static_cast<int>(env.season_progress) % 4;
        env.season_progress = fmod(env.season_progress, 1.0); // 当前季节内的进度

        // 季节影响环境
        switch (season) {
        case 0: // 春
            env.temperature = 10.0 + 15.0 * sin(env.season_progress * 3.1416);
            env.humidity = 70.0;
            env.daylight_hours = 12.0 + 4.0 * sin(env.season_progress * 3.1416);
            env.rainfall = 60.0;
            break;
        case 1: // 夏
            env.temperature = 20.0 + 15.0 * sin(env.season_progress * 3.1416);
            env.humidity = 60.0;
            env.daylight_hours = 16.0 - 4.0 * sin(env.season_progress * 3.1416);
            env.rainfall = 30.0;
            break;
        case 2: // 秋
            env.temperature = 10.0 + 10.0 * sin(env.season_progress * 3.1416);
            env.humidity = 50.0;
            env.daylight_hours = 12.0 - 4.0 * sin(env.season_progress * 3.1416);
            env.rainfall = 40.0;
            break;
        case 3: // 冬
            env.temperature = -5.0 + 10.0 * sin(env.season_progress * 3.1416);
            env.humidity = 40.0;
            env.daylight_hours = 8.0 + 4.0 * sin(env.season_progress * 3.1416);
            env.rainfall = 20.0;
            break;
        }

        // 温度波动
        env.temperature += (rand() % 7 - 3);

        // 降雨量波动
        env.rainfall = max(0.0, min(100.0, env.rainfall + (rand() % 20 - 10)));
    }

    // 更新天气
    void update_weather() {
        // 天气持续时间结束，生成新天气
        if (--env.weather_duration <= 0) {
            // 根据季节确定天气概率
            vector<WeatherType> weather_options;
            vector<double> weather_probs;

            if (season == 0) { // 春
                weather_options = { SUNNY, RAINY, CLOUDY, STORMY };
                weather_probs = { 0.4, 0.3, 0.2, 0.1 };
            }
            else if (season == 1) { // 夏
                weather_options = { SUNNY, CLOUDY, STORMY, DROUGHT };
                weather_probs = { 0.5, 0.2, 0.2, 0.1 };
            }
            else if (season == 2) { // 秋
                weather_options = { SUNNY, CLOUDY, RAINY };
                weather_probs = { 0.4, 0.3, 0.3 };
            }
            else { // 冬
                weather_options = { CLOUDY, SNOWY, SUNNY };
                weather_probs = { 0.5, 0.4, 0.1 };
            }

            // 选择天气
            double r = static_cast<double>(rand()) / RAND_MAX;
            double cumulative = 0.0;
            for (int i = 0; i < weather_options.size(); i++) {
                cumulative += weather_probs[i];
                if (r < cumulative) {
                    env.weather = weather_options[i];
                    break;
                }
            }

            // 设置天气持续时间 (1-5天)
            env.weather_duration = 1 + rand() % 5;
        }

        // 更新连续天气计数
        if (env.weather == RAINY || env.weather == STORMY) {
            env.consecutive_rain++;
            env.consecutive_sunny = 0;
        }
        else if (env.weather == SUNNY || env.weather == DROUGHT) {
            env.consecutive_sunny++;
            env.consecutive_rain = 0;
        }
        else {
            env.consecutive_rain = 0;
            env.consecutive_sunny = 0;
        }
    }

    // 更新积水、积雪和干旱
    void update_terrain_hydrology() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // 降雨增加积水
                if (env.weather == RAINY) {
                    terrain[y][x].water_accumulation = min(1.0,
                        terrain[y][x].water_accumulation + 0.05 * (env.rainfall / 50.0));
                }
                // 暴雨大幅增加积水
                else if (env.weather == STORMY) {
                    terrain[y][x].water_accumulation = min(1.0,
                        terrain[y][x].water_accumulation + 0.15 * (env.rainfall / 50.0));
                }
                // 晴天减少积水
                else if (env.weather == SUNNY) {
                    terrain[y][x].water_accumulation = max(0.0,
                        terrain[y][x].water_accumulation - 0.03);
                }
                // 干旱大幅减少积水
                else if (env.weather == DROUGHT) {
                    terrain[y][x].water_accumulation = max(0.0,
                        terrain[y][x].water_accumulation - 0.08);
                    terrain[y][x].drought_level = min(1.0,
                        terrain[y][x].drought_level + 0.05);
                }

                // 下雪增加积雪
                if (env.weather == SNOWY && env.temperature < 0) {
                    terrain[y][x].snow_depth = min(1.0,
                        terrain[y][x].snow_depth + 0.1);
                }
                // 晴天减少积雪
                else if (env.weather == SUNNY && env.temperature > 0) {
                    terrain[y][x].snow_depth = max(0.0,
                        terrain[y][x].snow_depth - 0.05);
                }

                // 连续晴天增加干旱
                if (env.consecutive_sunny > 5) {
                    terrain[y][x].drought_level = min(1.0,
                        terrain[y][x].drought_level + 0.01 * env.consecutive_sunny);
                }
                // 降雨减少干旱
                if (env.consecutive_rain > 0) {
                    terrain[y][x].drought_level = max(0.0,
                        terrain[y][x].drought_level - 0.02 * env.consecutive_rain);
                }

                // 积水过多形成洪水区
                if (terrain[y][x].water_accumulation > 0.5 &&
                    terrain[y][x].type != WATER && terrain[y][x].type != MARSH) {
                    terrain[y][x].type = FLOODED;
                }
                // 积水减少恢复原状
                else if (terrain[y][x].water_accumulation < 0.2 &&
                    terrain[y][x].type == FLOODED) {
                    // 根据原始高度恢复地形
                    double h = terrain[y][x].original_height;
                    if (h < 0.2) terrain[y][x].type = WATER;
                    else if (h < 0.25) terrain[y][x].type = BEACH;
                    else if (h < 0.3) terrain[y][x].type = MARSH;
                    else if (h < 0.5) {
                        if (terrain[y][x].water_level > 0.7) terrain[y][x].type = FOREST;
                        else terrain[y][x].type = PLAIN;
                    }
                    else if (h < 0.7) terrain[y][x].type = GRASSLAND;
                    else terrain[y][x].type = MOUNTAIN;
                }

                // 干旱导致水域缩小
                if (terrain[y][x].type == WATER && terrain[y][x].drought_level > 0.6) {
                    if (terrain[y][x].height > 0.15) {
                        terrain[y][x].type = MARSH;
                    }
                }
                else if (terrain[y][x].type == MARSH && terrain[y][x].drought_level > 0.7) {
                    terrain[y][x].type = PLAIN;
                }
            }
        }
    }

    // 处理环境灾难
    void apply_disaster() {
        if ((double)rand() / RAND_MAX < env.disaster_chance) {
            cout << "\n!!! 发生环境灾难 !!!\n";
            int disaster_type = rand() % 5;
            int casualties = static_cast<int>(organisms.size()) / 5;

            switch (disaster_type) {
            case 0: // 火灾
                cout << "森林火灾爆发!\n";
                for (int i = 0; i < casualties; i++) {
                    int index = rand() % static_cast<int>(organisms.size());
                    if (dynamic_cast<Plant*>(organisms[index]) ||
                        dynamic_cast<Tree*>(organisms[index])) {
                        delete organisms[index];
                        organisms.erase(organisms.begin() + index);
                    }
                }
                env.pollution = min(1.0, env.pollution + 0.1);
                break;

            case 1: // 洪水
                cout << "洪水泛滥!\n";
                for (int i = 0; i < casualties; i++) {
                    int index = rand() % static_cast<int>(organisms.size());
                    if (!organisms[index]->getIsAquatic()) {
                        delete organisms[index];
                        organisms.erase(organisms.begin() + index);
                    }
                }
                env.pollution = min(1.0, env.pollution + 0.05);
                // 增加水位
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        terrain[y][x].water_accumulation = min(1.0,
                            terrain[y][x].water_accumulation + 0.3);
                    }
                }
                break;

            case 2: // 瘟疫
                cout << "瘟疫蔓延!\n";
                env.disease = static_cast<DiseaseType>(1 + rand() % 3);
                env.disease_duration = 30; // 持续30天
                break;

            case 3: // 火山喷发
                cout << "火山喷发!\n";
                for (int i = 0; i < casualties; i++) {
                    int index = rand() % static_cast<int>(organisms.size());
                    delete organisms[index];
                    organisms.erase(organisms.begin() + index);
                }
                env.pollution = min(1.0, env.pollution + 0.3);
                // 增加火山地形
                for (int i = 0; i < 10; i++) {
                    int x = rand() % width;
                    int y = rand() % height;
                    if (terrain[y][x].height > 0.8) {
                        terrain[y][x].type = VOLCANIC;
                    }
                }
                break;

            case 4: // 干旱
                cout << "严重干旱!\n";
                env.weather = DROUGHT;
                env.weather_duration = 10;
                // 增加干旱程度
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        terrain[y][x].drought_level = min(1.0,
                            terrain[y][x].drought_level + 0.2);
                    }
                }
                break;
            }
        }

        // 处理疾病传播
        if (env.disease != NONE) {
            env.disease_duration--;
            if (env.disease_duration <= 0) {
                env.disease = NONE;
            }
            else {
                // 随机感染生物
                for (int i = 0; i < organisms.size() / 20; i++) {
                    int index = rand() % organisms.size();
                    organisms[index]->contract_disease(env.disease);
                }
            }
        }

        // 污染自然减少
        env.pollution = max(0.0, env.pollution - 0.005);

        // 地形自然恢复
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                terrain[y][x].pollution_level = max(0.0, terrain[y][x].pollution_level - 0.001);
                terrain[y][x].disease_level = max(0.0, terrain[y][x].disease_level - 0.002);

                // 肥力缓慢恢复
                if (terrain[y][x].fertility < 0.5) {
                    terrain[y][x].fertility = min(0.5, terrain[y][x].fertility + 0.0001);
                }
            }
        }
    }

    // 寄生关系处理
    void handle_parasites() {
        for (Organism* org : organisms) {
            // 寄生生物寻找宿主
            if (Parasite* parasite = dynamic_cast<Parasite*>(org)) {
                bool found_host = false;
                for (Organism* host : organisms) {
                    if (host != parasite && !dynamic_cast<Parasite*>(host) && !host->is_dead()) {
                        int dx = abs(parasite->getX() - host->getX());
                        int dy = abs(parasite->getY() - host->getY());
                        if (dx <= 1 && dy <= 1) {
                            // 移动到宿主位置
                            parasite->lose_energy(0.05); // 移动消耗能量
                            parasite->gain_energy(0.1);  // 找到宿主获得能量
                            found_host = true;
                            break;
                        }
                    }
                }

                // 没找到宿主会死亡
                if (!found_host) {
                    parasite->lose_energy(0.5);
                }
            }
        }
    }

public:
    World() : day(0), season(0), viewport_x(0), viewport_y(0),
        viewport_width(40), viewport_height(20), selected_x(-1), selected_y(-1), show_history(false) {
        srand(static_cast<unsigned int>(time(0)));
        // 初始化地形
        generate_terrain();
        // 初始化随机生物
        initialize_organisms();
    }

    ~World() {
        clear_organisms();
    }

    // 清空所有生物
    void clear_organisms() {
        for (Organism* org : organisms) {
            delete org;
        }
        organisms.clear();
    }

    // 初始化生物种群
    void initialize_organisms() {
        clear_organisms();

        // 植物
        for (int i = 0; i < 500; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Plant(x, y));
            }
        }

        // 树木
        for (int i = 0; i < 300; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Tree(x, y));
            }
        }

        // 水生植物
        for (int i = 0; i < 200; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y) && terrain[y][x].type == WATER) {
                organisms.push_back(new AquaticPlant(x, y));
            }
        }

        // 食草动物
        for (int i = 0; i < 80; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Herbivore(x, y));
            }
        }

        // 食肉动物
        for (int i = 0; i < 30; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Carnivore(x, y));
            }
        }

        // 杂食动物
        for (int i = 0; i < 40; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Omnivore(x, y));
            }
        }

        // 昆虫
        for (int i = 0; i < 200; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Insect(x, y));
            }
        }

        // 飞行昆虫
        for (int i = 0; i < 150; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new FlyingInsect(x, y));
            }
        }

        // 分解者
        for (int i = 0; i < 150; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Decomposer(x, y));
            }
        }

        // 顶级掠食者
        for (int i = 0; i < 10; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new ApexPredator(x, y));
            }
        }

        // 寄生生物
        for (int i = 0; i < 100; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Parasite(x, y));
            }
        }

        // 鱼类
        for (int i = 0; i < 100; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y) && terrain[y][x].type == WATER) {
                organisms.push_back(new Fish(x, y));
            }
        }

        // 鸟类
        for (int i = 0; i < 50; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Bird(x, y));
            }
        }

        // 爬行动物
        for (int i = 0; i < 40; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Reptile(x, y));
            }
        }

        // 两栖动物
        for (int i = 0; i < 60; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Amphibian(x, y));
            }
        }

        // 食腐动物
        for (int i = 0; i < 70; i++) {
            int x = rand() % width;
            int y = rand() % height;
            if (can_place_organism(x, y)) {
                organisms.push_back(new Scavenger(x, y));
            }
        }
    }

    // 检查位置是否可以放置生物
    bool can_place_organism(int x, int y) {
        if (x < 0 || y < 0 || x >= width || y >= height)
            return false;

        // 检查地形是否适合
        for (Organism* org : organisms) {
            if (org->getX() == x && org->getY() == y) {
                return false; // 位置已被占用
            }
        }

        return true;
    }

    // 重置世界
    void reset() {
        day = 0;
        season = 0;
        env = Environment();
        generate_terrain();
        initialize_organisms();
        selected_x = -1;
        selected_y = -1;
        show_history = false;
    }

    // 模拟一天的变化
    void simulate_day() {
        if (day >= max_days) return; // 达到最大天数

        day++;

        // 保存前一天状态
        for (Organism* org : organisms) {
            org->save_previous_state("存活");
        }

        // 更新季节
        update_season();

        // 更新天气
        update_weather();

        // 更新地形水文
        update_terrain_hydrology();

        // 环境灾难
        apply_disaster();

        // 生物行为
        vector<Organism*> new_organisms;
        for (Organism* org : organisms) {
            if (!org->is_dead()) {
                // 天气影响
                org->weather_effect(env, terrain[org->getY()][org->getX()]);

                org->move(terrain, env);
                org->eat(env, organisms, terrain);
                org->age_organism(env);

                // 繁殖
                Organism* child = org->reproduce();
                if (child) {
                    new_organisms.push_back(child);
                }
            }
        }

        // 添加新生物
        for (Organism* org : new_organisms) {
            organisms.push_back(org);
        }

        // 处理寄生关系
        handle_parasites();

        // 移除死亡的生物
        auto it = remove_if(organisms.begin(), organisms.end(),
            [](Organism* org) {
                if (org->is_dead()) {
                    delete org;
                    return true;
                }
                return false;
            });
        organisms.erase(it, organisms.end());

        // 自然演替 - 森林扩张
        if (day % 30 == 0) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (terrain[y][x].type == PLAIN && terrain[y][x].fertility > 0.6) {
                        // 检查周围是否有森林
                        for (int dy = -1; dy <= 1; dy++) {
                            for (int dx = -1; dx <= 1; dx++) {
                                int nx = x + dx;
                                int ny = y + dy;
                                if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
                                    if (terrain[ny][nx].type == FOREST) {
                                        if (rand() % 100 < 5) {
                                            terrain[y][x].type = FOREST;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 显示当前世界状态
    void display() const {
        system("cls");

        // 季节名称
        string seasons[4] = { "春", "夏", "秋", "冬" };

        // 天气名称
        string weather_names[6] = {
            "晴天", "雨天", "雪天", "多云", "暴风雨", "干旱"
        };

        // 天气颜色
        int weather_colors[6] = {
            COLOR_DEFAULT, COLOR_RAIN, COLOR_SNOW,
            COLOR_DEFAULT, COLOR_WARNING, COLOR_DROUGHT
        };

        // 标题
        SetColor(COLOR_TITLE);
        cout << " 生态系统模拟器 - 复杂生物网络 (1000x1000) ";
        SetColor(COLOR_DEFAULT);
        cout << endl;
        cout << "==================================================" << endl;

        // 环境信息
        SetColor(COLOR_STATS);
        cout << "天数: " << day << "/730 (" << seasons[season] << "季) | ";
        cout << "温度: " << fixed << setprecision(1) << env.temperature << "°C | ";
        cout << "湿度: " << fixed << setprecision(1) << env.humidity << "%" << endl;
        cout << "降雨: " << fixed << setprecision(1) << env.rainfall << "mm | ";
        cout << "白天时长: " << fixed << setprecision(1) << env.daylight_hours << "小时 | ";
        cout << "污染: " << fixed << setprecision(2) << env.pollution * 100 << "%" << endl;

        // 显示天气
        SetColor(weather_colors[env.weather]);
        cout << "天气: " << weather_names[env.weather]
            << " (剩余" << env.weather_duration << "天)";
        SetColor(COLOR_DEFAULT);
        cout << endl;

        SetColor(COLOR_DEFAULT);

        // 视口和生物信息
        SetColor(COLOR_HIGHLIGHTA);
        cout << "视口位置: (" << viewport_x << "," << viewport_y << ") | ";
        cout << "生物总数: " << organisms.size() << endl;
        SetColor(COLOR_DEFAULT);

        // 显示疾病状态
        if (env.disease != NONE) {
            SetColor(COLOR_WARNING);
            string disease_name;
            switch (env.disease) {
            case FUNGAL_INFECTION: disease_name = "真菌感染"; break;
            case VIRAL_OUTBREAK: disease_name = "病毒爆发"; break;
            case PARASITIC_INFESTATION: disease_name = "寄生虫感染"; break;
            default: disease_name = "未知疾病";
            }
            cout << "当前疫情: " << disease_name << " (剩余" << env.disease_duration << "天)" << endl;
            SetColor(COLOR_DEFAULT);
        }

        // 统计种群数量（改为int类型）
        int plants = 0, trees = 0, aqua_plants = 0, herbs = 0, carns = 0, omnis = 0;
        int insects = 0, fly_insects = 0, decomps = 0, apexes = 0, paras = 0;
        int fishes = 0, birds = 0, reptiles = 0, amphibians = 0, scavengers = 0;

        for (const Organism* org : organisms) {
            if (dynamic_cast<const Plant*>(org)) plants++;
            if (dynamic_cast<const Tree*>(org)) trees++;
            if (dynamic_cast<const AquaticPlant*>(org)) aqua_plants++;
            if (dynamic_cast<const Herbivore*>(org)) herbs++;
            if (dynamic_cast<const Carnivore*>(org)) carns++;
            if (dynamic_cast<const Omnivore*>(org)) omnis++;
            if (dynamic_cast<const Insect*>(org)) insects++;
            if (dynamic_cast<const FlyingInsect*>(org)) fly_insects++;
            if (dynamic_cast<const Decomposer*>(org)) decomps++;
            if (dynamic_cast<const ApexPredator*>(org)) apexes++;
            if (dynamic_cast<const Parasite*>(org)) paras++;
            if (dynamic_cast<const Fish*>(org)) fishes++;
            if (dynamic_cast<const Bird*>(org)) birds++;
            if (dynamic_cast<const Reptile*>(org)) reptiles++;
            if (dynamic_cast<const Amphibian*>(org)) amphibians++;
            if (dynamic_cast<const Scavenger*>(org)) scavengers++;
        }

        // 植物统计
        SetColor(COLOR_PLANT);
        cout << "植物: " << plants << " | 树木: " << trees << " | 水生植物: " << aqua_plants << endl;

        // 动物统计
        SetColor(COLOR_HERBIVORE);
        cout << "食草动物: " << herbs << " | ";
        SetColor(COLOR_CARNIVORE);
        cout << "食肉动物: " << carns << " | ";
        SetColor(COLOR_DEFAULT);
        cout << "杂食动物: " << omnis << endl;

        // 其他生物
        SetColor(COLOR_DEFAULT);
        cout << "昆虫: " << insects << " | 飞行昆虫: " << fly_insects << " | 分解者: " << decomps << endl;
        cout << "顶级掠食者: " << apexes << " | 寄生生物: " << paras << " | 鱼类: " << fishes << endl;
        cout << "鸟类: " << birds << " | 爬行动物: " << reptiles << " | 两栖动物: " << amphibians << endl;
        cout << "食腐动物: " << scavengers << endl;

        // 创建视口网格 - 改为存储地形符号和生物符号
        vector<vector<string>> terrain_grid(viewport_height, vector<string>(viewport_width, " "));
        vector<vector<string>> organism_grid(viewport_height, vector<string>(viewport_width, " "));
        vector<vector<bool>> has_organism(viewport_height, vector<bool>(viewport_width, false));

        // 放置地形符号
        for (int y = 0; y < viewport_height; y++) {
            for (int x = 0; x < viewport_width; x++) {
                int world_x = viewport_x + x;
                int world_y = viewport_y + y;

                if (world_x < width && world_y < height) {
                    switch (terrain[world_y][world_x].type) {
                    case WATER:
                        terrain_grid[y][x] = "~";
                        break;
                    case PLAIN:
                        terrain_grid[y][x] = ".";
                        break;
                    case FOREST:
                        terrain_grid[y][x] = "Y";
                        break;
                    case MOUNTAIN:
                        terrain_grid[y][x] = "^";
                        break;
                    case DESERT:
                        terrain_grid[y][x] = "d";
                        break;
                    case MARSH:
                        terrain_grid[y][x] = "m";
                        break;
                    case VOLCANIC:
                        terrain_grid[y][x] = "V";
                        break;
                    case SNOW:
                        terrain_grid[y][x] = "*";
                        break;
                    case GRASSLAND:
                        terrain_grid[y][x] = ",";
                        break;
                    case JUNGLE:
                        terrain_grid[y][x] = "J";
                        break;
                    case TUNDRA:
                        terrain_grid[y][x] = "T";
                        break;
                    case BEACH:
                        terrain_grid[y][x] = "=";
                        break;
                    case FLOODED:
                        terrain_grid[y][x] = "≈";
                        break;
                    }
                }
            }
        }

        // 放置生物符号
        for (const Organism* org : organisms) {
            int x = org->getX() - viewport_x;
            int y = org->getY() - viewport_y;

            if (x >= 0 && y >= 0 && x < viewport_width && y < viewport_height) {
                organism_grid[y][x] = org->getSymbol();
                has_organism[y][x] = true;
            }
        }

        // 绘制世界 - 使用方格格式
        for (int y = 0; y < viewport_height; y++) {
            for (int x = 0; x < viewport_width; x++) {
                int world_x = viewport_x + x;
                int world_y = viewport_y + y;

                // 高亮选中的生物
                if (show_history && selected_x == world_x && selected_y == world_y) {
                    SetColor(COLOR_HIGHLIGHTA);
                }
                else {
                    // 绘制地形背景
                    if (world_x < width && world_y < height) {
                        switch (terrain[world_y][world_x].type) {
                        case WATER:
                            SetColor(COLOR_WATER);
                            break;
                        case PLAIN:
                            SetColor(COLOR_PLANT);
                            break;
                        case FOREST:
                            SetColor(COLOR_FOREST);
                            break;
                        case MOUNTAIN:
                            SetColor(COLOR_MOUNTAIN);
                            break;
                        case DESERT:
                            SetColor(COLOR_DESERT);
                            break;
                        case MARSH:
                            SetColor(COLOR_MARSH);
                            break;
                        case VOLCANIC:
                            SetColor(COLOR_VOLCANIC);
                            break;
                        case SNOW:
                            SetColor(COLOR_SNOW);
                            break;
                        case GRASSLAND:
                            SetColor(COLOR_GRASSLAND);
                            break;
                        case JUNGLE:
                            SetColor(COLOR_JUNGLE);
                            break;
                        case TUNDRA:
                            SetColor(COLOR_TUNDRA);
                            break;
                        case BEACH:
                            SetColor(COLOR_BEACH);
                            break;
                        case FLOODED:
                            SetColor(COLOR_RAIN);
                            break;
                        }
                    }
                    else {
                        SetColor(COLOR_DEFAULT);
                    }
                }

                // 如果有生物，则绘制生物（使用更亮的颜色）
                if (organism_grid[y][x] != " ") {
                    // 根据生物类型设置颜色
                    if (organism_grid[y][x] == "P" || organism_grid[y][x] == "T" || organism_grid[y][x] == "A") {
                        SetColor(COLOR_PLANT + 8); // 更亮的绿色
                    }
                    else if (organism_grid[y][x] == "H") {
                        SetColor(COLOR_HERBIVORE + 8); // 更亮的黄色
                    }
                    else if (organism_grid[y][x] == "C") {
                        SetColor(COLOR_CARNIVORE + 8); // 更亮的红色
                    }
                    else if (organism_grid[y][x] == "X") {
                        SetColor(COLOR_WARNING + 8); // 更亮的红色
                    }
                    else if (organism_grid[y][x] == "~") {
                        SetColor(COLOR_WATER + 8); // 更亮的蓝色
                    }
                    else if (organism_grid[y][x] == "B") {
                        SetColor(COLOR_STATS + 8); // 更亮的青色
                    }
                    else {
                        SetColor(COLOR_DEFAULT + 8); // 更亮的白色
                    }
                    cout << "[" << organism_grid[y][x] << "]";
                }
                else {
                    // 没有生物，只显示地形
                    cout << " " << terrain_grid[y][x] << " ";
                }
            }
            SetColor(COLOR_DEFAULT);
            cout << endl;
        }

        // 显示选中的生物状态
        if (show_history && selected_x != -1 && selected_y != -1) {
            bool found = false;
            for (const Organism* org : organisms) {
                if (org->getX() == selected_x && org->getY() == selected_y) {
                    found = true;
                    SetColor(COLOR_HIGHLIGHTA);
                    cout << "\n=== 生物状态详情 ===" << endl;
                    SetColor(COLOR_DEFAULT);
                    cout << "类型: " << org->getName() << endl;
                    cout << "位置: (" << selected_x << ", " << selected_y << ")" << endl;
                    cout << "能量: " << fixed << setprecision(1) << org->getEnergy() << "/" << org->getEnergy() * 2 << endl;
                    cout << "年龄: " << org->get_previous_state().age << "天" << endl;
                    cout << "状态: " << org->get_previous_state().status << endl;

                    // 显示前一天状态变化
                    const auto& prev = org->get_previous_state();
                    cout << "\n--- 前一天状态变化 ---" << endl;
                    cout << "位置变化: (" << prev.x << ", " << prev.y << ") -> (" << org->getX() << ", " << org->getY() << ")" << endl;
                    cout << "能量变化: " << fixed << setprecision(1) << prev.energy << " -> " << org->getEnergy() << endl;
                    cout << "年龄变化: " << prev.age << " -> " << org->get_previous_state().age + 1 << endl;
                    break;
                }
            }
            if (!found) {
                SetColor(COLOR_WARNING);
                cout << "\n该位置没有存活的生物" << endl;
                SetColor(COLOR_DEFAULT);
            }
        }

        // 图例
        SetColor(COLOR_STATS);
        cout << "\n图例: ";
        SetColor(COLOR_DEFAULT);
        cout << "P=植物, T=树木, A=水生植物, H=食草动物, C=食肉动物\n";
        cout << "      O=杂食动物, I=昆虫, F=飞行昆虫, D=分解者, X=顶级掠食者\n"; // 修改顶级掠食者符号为X
        cout << "      *=寄生生物, ~=鱼类, B=鸟类, R=爬行动物, M=两栖动物\n"; // 修改鸟类符号为B

        // 地形图例
        SetColor(COLOR_STATS);
        cout << "地形: ";
        SetColor(COLOR_PLANT); cout << "."; SetColor(COLOR_DEFAULT); cout << "平原 ";
        SetColor(COLOR_FOREST); cout << "Y"; SetColor(COLOR_DEFAULT); cout << "森林 ";
        SetColor(COLOR_GRASSLAND); cout << ","; SetColor(COLOR_DEFAULT); cout << "草原 ";
        SetColor(COLOR_JUNGLE); cout << "J"; SetColor(COLOR_DEFAULT); cout << "丛林\n";
        cout << "      ";
        SetColor(COLOR_MOUNTAIN); cout << "^"; SetColor(COLOR_DEFAULT); cout << "山脉 ";
        SetColor(COLOR_DESERT); cout << "d"; SetColor(COLOR_DEFAULT); cout << "沙漠 ";
        SetColor(COLOR_TUNDRA); cout << "T"; SetColor(COLOR_DEFAULT); cout << "苔原 ";
        SetColor(COLOR_SNOW); cout << "*"; SetColor(COLOR_DEFAULT); cout << "雪地\n";
        cout << "      ";
        SetColor(COLOR_WATER); cout << "~"; SetColor(COLOR_DEFAULT); cout << "水域 ";
        SetColor(COLOR_MARSH); cout << "m"; SetColor(COLOR_DEFAULT); cout << "沼泽 ";
        SetColor(COLOR_VOLCANIC); cout << "V"; SetColor(COLOR_DEFAULT); cout << "火山 ";
        SetColor(COLOR_BEACH); cout << "="; SetColor(COLOR_DEFAULT); cout << "海滩 ";
        SetColor(COLOR_RAIN); cout << "≈"; SetColor(COLOR_DEFAULT); cout << "积水区\n";

        SetColor(COLOR_DEFAULT);
        cout << "==================================================" << endl;
    }

    // 调整环境参数
    void adjust_environment() {
        SetColor(COLOR_HIGHLIGHTA);
        cout << "\n调整环境参数:\n";
        SetColor(COLOR_DEFAULT);

        cout << "1. 温度 (" << env.temperature << "°C): ";
        cin >> env.temperature;
        cout << "2. 湿度 (" << env.humidity << "%): ";
        cin >> env.humidity;
        cout << "3. 灾难概率 (" << env.disaster_chance * 100 << "%): ";
        cin >> env.disaster_chance;
        cout << "4. 污染 (" << env.pollution * 100 << "%): ";
        cin >> env.pollution;
        cout << "5. 降雨量 (" << env.rainfall << "mm): ";
        cin >> env.rainfall;
    }

    // 移动视口
    void move_viewport(int dx, int dy) {
        viewport_x = max(0, min(width - viewport_width, viewport_x + dx));
        viewport_y = max(0, min(height - viewport_height, viewport_y + dy));
        show_history = false; // 移动视口时关闭历史显示
    }

    // 选择生物查看历史
    void select_organism(int x, int y) {
        selected_x = viewport_x + x;
        selected_y = viewport_y + y;
        show_history = true;
    }

    // 获取生物数量
    size_t get_organism_count() const {
        return organisms.size();
    }

    // 获取当前天数
    int get_day() const {
        return day;
    }
};

// 显示欢迎界面
void display_welcome() {
    SetColor(COLOR_TITLE);
    cout << "==================================================" << endl;
    cout << "        生态系统模拟器 - 复杂生物网络 (中文版)      " << endl;
    cout << "==================================================" << endl;
    SetColor(COLOR_DEFAULT);

    cout << "这是一个模拟自然界中复杂生物网络的程序，包含多种生物" << endl;
    cout << "种群和复杂的生态关系，如食物链、共生和寄生关系。" << endl;

    SetColor(COLOR_STATS);
    cout << "\n模拟规则:" << endl;
    SetColor(COLOR_DEFAULT);
    cout << "- 植物通过光合作用获取能量并繁殖" << endl;
    cout << "- 食草动物吃植物，食肉动物吃食草动物" << endl;
    cout << "- 杂食动物既吃植物又吃小动物" << endl;
    cout << "- 昆虫吃植物和腐肉，分解者分解死亡生物" << endl;
    cout << "- 顶级掠食者捕食其他动物" << endl;
    cout << "- 寄生生物依附在其他生物身上获取能量" << endl;
    cout << "- 环境因素（温度、湿度、污染）影响所有生物" << endl;
    cout << "- 季节变化影响环境条件" << endl;
    cout << "- 地形影响生物分布和行为" << endl;
    cout << "- 天气系统影响环境（晴天、雨天、雪天、暴风雨、干旱）" << endl;
    cout << "- 积水、积雪和干旱会影响生物的生存和繁衍" << endl;

    SetColor(COLOR_HIGHLIGHTA);
    cout << "\n本次模拟将持续730天（约两年）" << endl;
    SetColor(COLOR_STATS);
    cout << "\n操作说明:" << endl;
    SetColor(COLOR_DEFAULT);
    cout << "  S - 模拟一天" << endl;
    cout << "  A - 调整环境参数" << endl;
    cout << "  R - 重置模拟" << endl;
    cout << "  Q - 退出程序" << endl;
    cout << "  方向键 - 移动视口" << endl;
    cout << "  C - 选择生物查看状态变化" << endl;

    SetColor(COLOR_TITLE);
    cout << "==================================================" << endl;
    SetColor(COLOR_DEFAULT);
    cout << "按任意键开始模拟..." << endl;
    _getch();
}

// 主函数
int main() {
    // 设置控制台支持中文
    SetConsoleToGB2312();

    // 显示欢迎界面
    display_welcome();

    World world;

    while (true) {
        world.display();

        // 检查是否达到最大天数
        if (world.get_day() >= 730) {
            SetColor(COLOR_HIGHLIGHTA);
            cout << "\n模拟完成！已到达730天（两年）。" << endl;
            SetColor(COLOR_DEFAULT);
            cout << "按任意键退出..." << endl;
            _getch();
            break;
        }

        SetColor(COLOR_STATS);
        cout << "\n选项: [S]模拟一天  [A]调整环境  [R]重置  [Q]退出  [方向键]移动视口  [C]选择生物\n";
        SetColor(COLOR_DEFAULT);
        cout << "请选择操作: ";
        char choice;
        choice = _getch();

        if (toupper(choice) == 'Q') break;

        if (toupper(choice) == 'A') {
            world.adjust_environment();
        }
        else if (toupper(choice) == 'R') {
            // 重置世界
            world.reset();
        }
        else if (toupper(choice) == 'S') {
            world.simulate_day();
        }
        else if (toupper(choice) == 'C') {
            int sel_x, sel_y;
            cout << "\n输入要查看的生物坐标 (相对于视口): ";
            cin >> sel_x >> sel_y;
            world.select_organism(sel_x, sel_y);
        }
        else if (choice == -32) { // 扩展键
            choice = _getch(); // 获取第二个键值
            switch (choice) {
            case 72: world.move_viewport(0, -10); break; // 上
            case 80: world.move_viewport(0, 10); break;  // 下
            case 75: world.move_viewport(-10, 0); break; // 左
            case 77: world.move_viewport(10, 0); break;  // 右
            }
        }

        // 如果所有生物灭绝，结束模拟
        if (world.get_organism_count() == 0) {
            world.display();
            SetColor(COLOR_WARNING);
            cout << "\n所有生物已灭绝，模拟结束!\n";
            SetColor(COLOR_DEFAULT);
            break;
        }
    }

    SetColor(COLOR_TITLE);
    cout << "\n感谢使用生态系统模拟器!\n";
    SetColor(COLOR_DEFAULT);
    return 0;
}