#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <numeric>
#include <cmath>
#include <fstream>
#include <set>
#include <functional>

//! @note WOW NICE INDENTATION BRO

namespace {
    
    using i64 = std::int64_t; using u64 = std::uint64_t;
    using i32 = std::int32_t; using u32 = std::uint32_t;

    struct point_t {
        i64 x, y;
        friend std::ostream& operator<<(std::ostream&o, point_t const& point) { return o << point.x << ' ' << point.y; }        
        friend bool operator<(const point_t& lhs, const point_t& rhs) { return (lhs.x < rhs.x) || ((!(rhs.x < lhs.x)) && (lhs.y < rhs.y)); }
    };

    template <typename T> std::ostream& operator<<(std::ostream& o, std::vector<T> const& vector) {
        for (const auto& element: vector) o << element << " "; o << "\n";
        return o;
    }
}

namespace utility {

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
    T random_integral
    (T lower_bound = std::numeric_limits<T>::min(), T upper_bound = std::numeric_limits<T>::max()) {
        static std::mt19937_64 generator {std::random_device {}()};
        static std::uniform_int_distribution<T> distribution (lower_bound, upper_bound);
        return distribution(generator);
    }

    template <typename C> std::ostream& container_to_io(C const& container, std::ostream& ostream) {
        std::for_each(std::begin(container), std::end(container),
            [&ostream] (auto const& element)
            { ostream << element << "\n"; });
        return ostream;
    }

}


namespace quick_hull_algorithm {
    
    auto quick_hull_finder(std::vector<point_t> const& points) {

        std::set<point_t> hull;

        const auto cross_product = [&] (point_t const& a, point_t const& b, point_t const& lp) {
            //! @note Here we abuse the cross product to determine how far the point lies from the line.
            const auto distance = (lp.x - a.x) * (b.y - a.y) - (lp.y - a.y) * (b.x - a.x);
            const auto sign = distance == 0? 0: distance < 0? -1: 1;
            //! @note If
            //!       (sign  < 0),  then point lies on the one side of line
            //!       (sign == 0),  then point lies exactly on the line
            //!       (sign  > 0),  then point lies on the another side of line
            return std::pair {sign, std::abs(distance)};
        };

        std::function<void(point_t const&, point_t const&, i32)> find_hull;
        find_hull = [&points, &cross_product, &hull, &find_hull] (point_t const& x, point_t const& y, i32 side) {
            i32 farthest = std::numeric_limits<i32>::max(), maximum = 0;
            for (std::size_t index = 0; index < points.size(); ++index) {
                if (const auto [sign, distance] = cross_product(x, y, points[index]); 
                    sign == side && distance > maximum) {
                    farthest = index; maximum = distance;
                }
            }
            if (farthest == std::numeric_limits<i32>::max())
            { 
                hull.emplace(x); 
                hull.emplace(y); 
                return; 
            }
            find_hull(points[farthest], x, (side == 1) ? -1:  1);
            find_hull(points[farthest], y, (side == 1) ?  1: -1);  
        };
        auto [min, max] = std::minmax_element(std::begin(points), std::end(points), 
                                              [&](point_t const& a, point_t const& b) { return a.x < b.x; });
        find_hull(*min, *max, 1); find_hull(*min, *max, -1);
        return hull;
    };
}

int main() {
    std::vector<point_t> points;
    std::cout << "[qhull] Please, set the number of randomly arranged points.\n";
    i32 n = (static_cast<void>(std::cin >> n), n);
    for (std::size_t i = 0; i < n; ++i) {
        point_t temporary = {utility::random_integral(-300, 300), utility::random_integral(-300, 300)};
        points.push_back(temporary);
    }
    std::cout << "[qhull] Trying to build the convex hull ...\n";
    auto hull_points = quick_hull_algorithm::quick_hull_finder(points);
    std::cout << "[qhull] Dumping the results to I/O ...\n";
    std::ofstream p {"pts.txt"}, h {"h.txt"};
    utility::container_to_io(points, p); utility::container_to_io(hull_points, h);
    std::cout << "[qhull] Success!\n";
    for (const auto& el: hull_points) std::cout << el << "\n";
    return 0;
}
