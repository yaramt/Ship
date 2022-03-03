#include <vector>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <string>
#include <optional>
#include <iostream>

// shipping.
namespace shipping {
    template<typename T> class NamedType {
    public:
        T t;
        explicit NamedType(T t) : t(t) {}
        operator T() const {
            return t;
        }
    };

    struct X : NamedType<int> {
        using NamedType<int>::NamedType;
    };

    struct Y : NamedType<int> {
        using NamedType<int>::NamedType;
    };

    struct Height : NamedType<int> {
        using NamedType<int>::NamedType;
    };
    using Position = std::tuple<shipping::X, shipping::Y, shipping::Height>;
}

// define hash for Position
namespace std
{
    template<> struct hash<shipping::Position>
    {
        std::size_t operator()(const shipping::Position& pos) const noexcept
        {
            return ((std::get<0>(pos) ^ (std::get<1>(pos) << 1)) >> 1) ^ (std::get<2>(pos) << 1);
        }
    };
}

// back to shipping 
namespace shipping {

    bool sortbyxy(const std::tuple<X, Y, Height>& a, const std::tuple<X, Y, Height>& b)
    {
        if ((std::get<0>(a)) != (std::get<0>(b)))
            return (std::get<0>(a)) < (std::get<0>(b));
        return (std::get<1>(a)) <= (std::get<1>(b));
    }

    struct BadShipOperationException {
        BadShipOperationException(X x, Y y, Height height, const std::string& msg) {
            std::cout << msg << " : X {" << x << "}, Y {" << y << "}, Height {" << height << "}\n";
        }
        BadShipOperationException(X x, Y y, const std::string& msg) {
            std::cout << msg << " : X {" << x << "}, Y {" << y << "}\n";
        }
    };

    template<typename Container>
    using Grouping = std::unordered_map<std::string, std::function<std::string(const Container&)>>;

    template<typename Container>
    class Ship {
        class GroupView {
            const std::unordered_map<Position, const Container&>* p_group = nullptr;
            using iterator_type = typename std::unordered_map<Position, const Container&>::const_iterator;
        public:
            GroupView(const std::unordered_map<Position, const Container&>& group) : p_group(&group) {}
            GroupView(int) {}
            auto begin() const {
                return p_group ? p_group->begin() : iterator_type{};
            }
            auto end() const {
                return p_group ? p_group->end() : iterator_type{};
            }
        };

        class iterator {
        public:
            using ContainersItr = typename std::vector<std::optional<Container>>::const_iterator;
            ContainersItr containers_itr;
            ContainersItr containers_end;
            void set_itr_to_occupied_sit() {
                while (containers_itr != containers_end && !(*containers_itr)) {
                    ++containers_itr;
                }
            }
            iterator(ContainersItr containers_itr, ContainersItr containers_end)
                : containers_itr(containers_itr), containers_end(containers_end) {
                set_itr_to_occupied_sit();
            }
            iterator operator++() {
                ++containers_itr;
                set_itr_to_occupied_sit();
                return *this;
            }
            const Container& operator*() const {
                return containers_itr->value();
            }
            bool operator!=(iterator other) const {
                return containers_itr != other.containers_itr;
            }
        };

        class iterator_Position {
            using ContainersItr = typename std::vector<std::optional<Container>>::const_iterator;
            ContainersItr containers_itr;
            ContainersItr containers_end;
            ContainersItr containers_real_end;
            int dif;
            void set_itr_to_occupied_sit() {
                while (containers_itr != containers_end && !(*containers_itr)) {
                    containers_itr = containers_itr - dif;
                }
                if (containers_itr == containers_end && !(*containers_itr))
                    containers_itr = containers_real_end;
            }
        public:
            iterator_Position(ContainersItr containers_itr, ContainersItr containers_end, ContainersItr containers_real_end, int dif)
                : containers_itr(containers_itr), containers_end(containers_end), containers_real_end(containers_real_end), dif(dif) {
                set_itr_to_occupied_sit();
            }
            iterator_Position operator++() {
                if (containers_itr != containers_end) {
                    containers_itr = containers_itr - dif;
                    set_itr_to_occupied_sit();
                }
                else containers_itr = containers_real_end;
                return *this;
            }
            const Container& operator*() const {
                return containers_itr->value();
            }
            bool operator!=(iterator_Position other) const {
                return containers_itr != other.containers_itr;
            }
        };

        class PositionView {
            const std::vector<std::optional<Container>>* p_column = nullptr;
            int x;
            int y;
            int x_size;
            int y_size;
            int height_size;
        public:
            PositionView(const std::vector<std::optional<Container>>& column, int x, int y, int x_size, int y_size, int height_size) : p_column(&column), x(x), y(y), x_size(x_size), y_size(y_size), height_size(height_size) {}
            PositionView(int) {}
            iterator_Position begin() const {//{upper elememnt in column, lowest element in column, difference i n indexes to move in same column}
                return  { p_column->begin() + (height_size - 1) * y_size * x_size + y * x_size + x , p_column->begin() + y * x_size + x ,p_column->end(), y_size * x_size };
            }
            iterator_Position end() const {
                return  { p_column->end(), p_column->end(), p_column->end() ,0 };
            }
        };

        std::vector<std::optional<Container>> containers;
        int x_size;
        int y_size;
        int height_size;
        std::vector<std::tuple<X, Y, Height>> restrictions;
        std::vector<std::tuple<X, Y, Height>> restrictions_copy;
        Grouping<Container> groupingFunctions;
        using Pos2Container = std::unordered_map<Position, const Container&>;
        using Group = std::unordered_map<std::string, Pos2Container>;
        mutable std::unordered_map<std::string, Group> groups;
        void check_xy(X x, Y y)const {
            if (x < 0 || x >= x_size || y < 0 || y >= y_size)
                throw BadShipOperationException(x, y, "index out of range");
        }
        int pos_index(X x, Y y, Height height) const {
            check_xy(x, y);
            return height * y_size * x_size + y * x_size + x;
        }
        Container& get_container(X x, Y y, Height height) {
            return containers[pos_index(x, y, height)].value();
        }
        //return how much space is blocked in column x,y
        Height findBlockedSpace(X x, Y y) {
            auto it = std::find_if(restrictions.begin(), restrictions.end(), [x, y](const std::tuple<X, Y, Height>& e) {return ((std::get<0>(e) == x) && (std::get<1>(e) == y)); });
            if (it == restrictions.end())
                return Height{ 0 };
            return Height{ height_size - std::get<2>(*it) };
        }
        void addContainerToGroups(X x, Y y, Height height) {
            Container& e = get_container(x, y, height);
            height.t = height.t - findBlockedSpace(x, y).t;
            for (auto& group_pair : groupingFunctions) {
                groups[group_pair.first][group_pair.second(e)].insert({ std::tuple{x, y, height}, e });
            }
        }
        void removeContainerFromGroups(X x, Y y, Height height) {
            Container& e = get_container(x, y, height);
            height.t = height.t - findBlockedSpace(x, y).t;
            for (auto& group_pair : groupingFunctions) {
                groups[group_pair.first][group_pair.second(e)].erase(std::tuple{ x, y, height });
            }
        }
        void addToRestrictionCopy(X x, Y y, Height height) {
            if (height == height_size) {
                restrictions_copy.emplace_back(x, y, Height{ height_size - 1 });
                sort(restrictions_copy.begin(), restrictions_copy.end(), sortbyxy);
            }
        }
        void search_restrictions_copy(X& x, Y& y, Height& height) {
            for (auto& [x_r, y_r, height_r] : restrictions_copy) {
                if (x_r == x && y_r == y) {
                    if (height_r == 0)
                        throw BadShipOperationException(x, y, "Bad load: no room at this location");
                    height = height_r;
                    height_r.t = height_r - 1;
                    break;
                }
            }
        }
        void update_unload_restrictions_copy(std::vector<std::tuple<X, Y, Height>>::iterator& it) {
            std::get<2>(*it).t = std::get<2>(*it) + 1;
            if (std::get<2>(*it) == height_size)
                restrictions_copy.erase(it);
        }
        void init_restrictions() {
            restrictions_copy = restrictions;
            sort(restrictions_copy.begin(), restrictions_copy.end(), sortbyxy);
            int x_prev = -1, y_prev = -1;
            for (auto const& [x_r, y_r, height_r] : restrictions_copy) {
                if (x_r < 0 || x_r >= x_size || y_r < 0 || y_r >= y_size || height_r < 0 || height_r >= height_size)
                    throw BadShipOperationException(x_r, y_r, height_r, "Restriction index out of range");
                if (x_prev == x_r && y_prev == y_r)
                    throw BadShipOperationException(x_r, y_r, height_r, "Error: Duplicate restrictions");
                x_prev = x_r;
                y_prev = y_r;
            }
        }

    public:
        //Ship’s Constructor (1):
        Ship(X x, Y y, Height max_height,
            std::vector<std::tuple<X, Y, Height>> restrictions,
            Grouping<Container> groupingFunctions)noexcept(false) :containers(x.t* y.t* max_height.t), x_size(x), y_size(y), height_size(max_height), restrictions(restrictions), groupingFunctions(groupingFunctions) {
            init_restrictions();
        }
        //Ship’s Constructor(2) :
        Ship(X x, Y y, Height max_height,
            std::vector<std::tuple<X, Y, Height>> restrictions) noexcept(false) :containers(x.t* y.t* max_height.t), x_size(x), y_size(y), height_size(max_height), restrictions(restrictions) {
            init_restrictions();
        }
        //Ship’s Constructor(3) :
        Ship(X x, Y y, Height max_height) noexcept :containers(x.t* y.t* max_height.t), x_size(x), y_size(y), height_size(max_height) {
        }

        //loading a container:
        void load(X x, Y y, Container c) noexcept(false) {
            check_xy(x, y);
            Height height{ height_size };
            search_restrictions_copy(x, y, height);
            addToRestrictionCopy(x, y, height);
            height.t = height_size - height;
            auto& pos = containers[pos_index(x, y, height)];
            pos = std::move(c);
            addContainerToGroups(x, y, height);
        }

        //unloading a container :
        Container unload(X x, Y y) noexcept(false) {
            check_xy(x, y);
            auto it = std::find_if(restrictions_copy.begin(), restrictions_copy.end(), [x, y](const std::tuple<X, Y, Height>& e) {return ((std::get<0>(e) == x) && (std::get<1>(e) == y)); });
            if (it == restrictions_copy.end())
                throw BadShipOperationException(x, y, "no container to unload");
            Height height{ height_size - std::get<2>(*it) - 1 };
            auto& pos = containers[pos_index(x, y, height)];
            if (!pos) {//if this position is a blocked position (according to restrictions_copy)
                throw BadShipOperationException(x, y, "no container to unload");
            }
            removeContainerFromGroups(x, y, height);
            update_unload_restrictions_copy(it);
            auto container = std::optional<Container>{};
            std::swap(pos, container);
            return container.value();
        }

        //moving a container from one location to another on the ship :
        void move(X from_x, Y from_y, X to_x, Y to_y) noexcept(false) {
            if ((int)from_x == (int)to_x && (int)from_y == (int)to_y)
                return;
            check_xy(to_x, to_y);
            auto it = std::find_if(restrictions_copy.begin(), restrictions_copy.end(), [to_x, to_y](const std::tuple<X, Y, Height>& e) {return ((std::get<0>(e) == to_x) && (std::get<1>(e) == to_y)); });
            if (it != restrictions_copy.end() && get<2>(*it) == 0)
                throw BadShipOperationException(to_x, to_y, "move error: -to- location has no room left");
            load(to_x, to_y, unload(from_x, from_y));
        }

        PositionView getContainersViewByPosition(X x, Y y) const {
            check_xy(x, y);
            return PositionView{ containers,x,y,x_size,y_size,height_size };
        }

        GroupView getContainersViewByGroup(const std::string& groupingName, const std::string& groupName) const {
            auto itr = groups.find(groupingName);
            if (itr == groups.end() && groupingFunctions.find(groupingName) != groupingFunctions.end()) {
                std::tie(itr, std::ignore) = groups.insert({ groupingName, Group{} });
            }
            if (itr != groups.end()) {
                const auto& grouping = itr->second;
                auto itr2 = grouping.find(groupName);
                if (itr2 == grouping.end()) {
                    std::tie(itr2, std::ignore) = itr->second.insert({ groupName, Pos2Container{} });
                }
                return GroupView{ itr2->second };
            }
            return GroupView{ 0 };
        }

        iterator begin() const {
            return { containers.begin(), containers.end() };
        }
        iterator end() const {
            return { containers.end(), containers.end() };
        }
        Ship(const Ship&) = delete;
        Ship& operator=(const Ship&) = delete;
        Ship(Ship&&) = default;
        Ship& operator=(Ship&&) = default;
    };
}

