#include "utils.hpp"


#include <set>
#include <assert.h>
#include <iostream>


struct Dar {

};

std::ostream& operator << (std::ostream& out, const Dar& dar) {
    out << "Dar";
    return out;
}

namespace std {
std::string to_string(const Dar& dar) {
    return "Dar";
}
} // namespace std


int main() {
    using namespace std;

    const vector<int> v1 = {1,2,3,4,5,1,3,9,0,1,5};
    const set<int> s1 = {1,2,3,4,5,1,3,9,0,1,5};

    assert((utils::sort(v1) ==
            vector<int>{0,1,1,1,2,3,3,4,5,5,9}));

    assert((utils::sort(vector<int>{1,23,4}) ==
            vector<int>{1,4,23}));

    assert((utils::sort([](const int& i, const int& j){return i>j;},
                        vector<int>{1,23,4}) ==
            vector<int>{23,4,1}));

    assert((utils::filter([](const int i){return i == 1 ? true : false;},
                          vector<int>{1,2,3}) ==
            vector<int>{1}));

    assert((utils::filter([](const int i){return i == 1 ? true : false;},
                          s1) ==
            set<int>{1}));

    {
        std::vector<int> t {1,2,3,4,5,6};
        utils::nocopy::filter([](const int i){return i == 1 ? true : false;}, t);
        assert((t == std::vector<int>{1}));
    }

    {
        std::vector<int> t {1,2,3};
        utils::nocopy::map([](const int i){return i*i;}, t);
        assert((t == std::vector<int>{1,4,9}));
    }

    // from vector<int> to vector<double> smart casting by lambda return type
    assert((utils::map([](const int&i){return(double)i*i;},
                       v1) ==
            vector<double>{1,4,9,16,25,1,9,81,0,1,25}));

    // from set<int> to set<double> smart casting by lambda return type
    assert((utils::map([](const int& i){return (double)i*i;},
                       s1) ==
            set<double>{0,1,4,9,16,25,81}));

    assert((utils::combine(v1, v1) ==
            vector<int>{1,2,3,4,5,1,3,9,0,1,5,1,2,3,4,5,1,3,9,0,1,5}));

    assert((utils::combine(v1, s1) ==
            vector<int>{1,2,3,4,5,1,3,9,0,1,5,0,1,2,3,4,5,9}));

    //assert((utils::combine(vector<int>{1,2}, set<float>{3,4}) == vector<int>{1,2,3,4}));//err

    assert((utils::combine(set<int>(), v1, s1) ==
            set<int>{0,1,2,3,4,5,9}));

    assert((utils::combine(vector<int>{1,2}, 4, set<int>{8,9}) ==
            vector<int>{1,2,4,8,9}));

    assert(utils::combine(v1) ==
           v1);

    assert((utils::combine(string{"123"}, vector<char>{'4'}, '5', "6") == "123456"));
    assert((utils::combine(vector<char>{'4'}, string{"12"}, '3') == vector<char>{'4','1','2','3'}));

    assert(utils::contains(vector<int>{1,2,3}, 1));
    assert(utils::contains(set<int>{1,2,3}, 1));
    assert(utils::contains(vector<int>{1,2,3}, vector<int>{1,2,3}));
    assert(utils::contains(vector<int>{1,2,3}, vector<int>{2,3}));
    assert(utils::contains(vector<int>{1,2,3}, vector<int>{2}));
    assert(utils::contains(vector<int>{1,2,3}, vector<int>{}));
    assert(!utils::contains(vector<int>{1,2,3}, vector<int>{4}));
    assert(!utils::contains(vector<int>{1,2,3}, vector<int>{2,3,4}));
    assert(!utils::contains(vector<int>{1,2,3}, set<int>{2,3,4}));
    assert(utils::contains(vector<int>{1,2,3}, set<int>{2,3}));
    assert(!utils::contains(vector<int>{}, set<int>{2,3}));
    assert(utils::contains(vector<int>{}, set<int>{}));
    assert(utils::contains(std::string{"123"}, set<char>{'1','2','3'}));
    assert(!utils::contains(std::string{"123"}, set<char>{'1','2','3','4'}));
    assert(utils::contains("123", set<char>{'1','2'}));
    assert(!utils::contains("123", set<char>{'1','2','3','4'}));
    assert(utils::contains("12345", "234"));


    // assert(utils::contains(s1, '1')); // compile-time error
    assert(utils::contains(std::string{"12345"}, '1'));
    assert(utils::contains("12345", '1'));
    // assert(utils::contains("12345", 1)); // compile-time error
    assert(utils::find(std::string{"12345"}, '3') != nullptr);
    // assert(utils::find(std::string{"12345"}, 3) != nullptr); // compile-time error
    assert(utils::find(v1, 9) != nullptr);
    assert(utils::find_if([](const int& i){return i > 3 ? true : false;}, v1) != nullptr);
    assert(utils::find_if([](const int& i){return i > 300 ? true : false;}, v1) == nullptr);
    assert(!utils::empty(v1));
    assert(utils::front(vector<int>{1,2,3}) == 1);
    assert(utils::front(set<int>{1,2,3}) == 1);
    assert(utils::front("12345") == '1');
    assert(utils::back(vector<int>{1,2,3}) == 3);
    assert(utils::back(set<int>{1,2,3}) == 3);
    assert(utils::back("12345") == '5');
    assert(utils::get(v1, 9) == 9);
    // assert(utils::get(v1, '9') == 9); // compile-time error
    assert(utils::get("12345", '3') == '3');
    assert(utils::get(std::string{"12345"}, '3') == '3');
    // assert(utils::get("12345", 3) == '3'); // compile-time error
    assert(utils::nth(v1, 4) == 5);
    assert(utils::is_sorted(vector<int>{1,2,3}));
    assert(utils::is_sorted(std::string{"12345"}));
    assert(!utils::is_sorted(std::string{"54321"}));
    assert(utils::is_sorted(""));
    assert(utils::is_sorted("2"));
    assert(utils::is_sorted("12345"));
    assert(!utils::is_sorted("412345"));
    assert(not utils::is_sorted(vector<int>{3,1,2,3}));

    assert((utils::push_front(vector<int>{1,2,3}, 4) == vector<int>{4,1,2,3}));
    assert((utils::push_front(set<int>{1,2,3}, 4) == set<int>{1,2,3,4}));

    assert((utils::push_back(vector<int>{1,2,3}, 4) == vector<int>{1,2,3,4}));
    assert((utils::push_back(set<int>{1,2,3}, 4) == set<int>{1,2,3,4}));

    {
        vector<int> v{1,2,3};
        utils::nocopy::pop_back(v);
        assert((v == vector<int>{1,2}));
    }

    {
        std::string s {"12345"};
        utils::nocopy::pop_back(s);
        assert(s == "1234");
    }

    assert((utils::pop_back(vector<int>{1,2,3}) == vector<int>{1,2}));
    assert((utils::pop_back(set<int>{1,2,3}) == set<int>{1,2}));
    assert((utils::pop_back(set<int>{1}) == set<int>{}));
    assert(utils::pop_back("12345") == "1234");

    assert((utils::pop_front(vector<int>{1,2,3}) == vector<int>{2,3}));
    assert((utils::pop_front(set<int>{1,2,3}) == set<int>{2,3}));
    assert((utils::pop_front(set<int>{1}) == set<int>{}));
    assert(utils::pop_front("12345") == "2345");

    assert((utils::insert(vector<int>{1,2,3}, 0, 111) == vector<int>{111,1,2,3}));
    assert((utils::insert(vector<int>{1,2,3}, 1, 111) == vector<int>{1,111,2,3}));
    assert((utils::insert(vector<int>{1,2,3}, 2, 111) == vector<int>{1,2,111,3}));
    assert((utils::insert(vector<int>{1,2,3}, 3, 111) == vector<int>{1,2,3,111}));
    assert((utils::insert(vector<int>{1,2,3}, 4, 111) == vector<int>{1,2,3,111}));

    assert((utils::insert(set<int>{1,2,3}, 4, 111) == set<int>{1,2,3,111}));
    assert((utils::insert(set<int>{1,2,3}, 4, 111) == set<int>{1,111,2,3}));

    assert((utils::remove_n(vector<int>{1,2,3}, 0) == vector<int>{2,3}));
    assert((utils::remove_n(vector<int>{1,2,3}, 1) == vector<int>{1,3}));
    assert((utils::remove_n(vector<int>{1,2,3}, 2) == vector<int>{1,2}));
    assert((utils::remove_n(vector<int>{1,2,3}, 3) == vector<int>{1,2,3}));
    assert((utils::remove_n(set<int>{1,2,3}, 2) == set<int>{1,2}));
    assert((utils::remove_n(vector<int>{1}, 0) == vector<int>{}));
    assert((utils::remove_n(vector<int>{}, 0) == vector<int>{}));

    assert((utils::remove(vector<int>{}, 0) == vector<int>{}));
    assert((utils::remove(vector<int>{1,2,3}, 2) == vector<int>{1,3}));
    assert((utils::remove(set<int>{1,2,3}, 1) == set<int>{2,3}));

    assert(utils::remove("12345", '3') == "1245");
    assert(utils::remove_n("12345", 2) == "1245");
    assert(utils::push_front("12345", '6') == "612345");
    assert(utils::push_back("12345", '6') == "123456");
    assert(utils::push_back("12345", "67", '8') == "12345678");
    assert(utils::insert("12345", 2, '6') == "126345");

    assert(utils::insert("12345", 0, "67") == "6712345");
    assert(utils::insert("12345", 2, "67") == "1267345");
    assert(utils::insert("12345", 200, "67") == "1234567");
    assert(utils::insert(std::string{"12345"}, 200, vector<char>{'6','7'}) == "1234567");
    assert((utils::insert(vector<char>{'1','2'}, 200, std::string{"34567"}) ==
            vector<char>{'1','2','3','4','5','6','7'}));

    assert((utils::insert(vector<int>{1,2,3,4,5}, 2, vector<int>{6,7}) ==
            vector<int>{1,2,6,7,3,4,5}));

    {
        vector<int> vx{1,2,3,4,5};
        assert((utils::insert(vx, 2, vector<int>{6,7}) == vector<int>{1,2,6,7,3,4,5}));
    }

    {
        const vector<int> vx{1,2,3,4,5};
        assert((utils::insert(vx, 2, vector<int>{6,7}) == vector<int>{1,2,6,7,3,4,5}));
    }

    {
        vector<int> vx{1,2,3,4,5};
        utils::nocopy::insert(vx, 2, vector<int>{6,7});
        assert((vx == vector<int>{1,2,6,7,3,4,5}));
    }

    { // compile-time error
        // const vector< int> vx{1,2,3,4,5};
        // utils::nocopy::insert(vx, 2, vector< int>{6,7});
        // assert((vx == vector<int>{1,2,6,7,3,4,5}));
    }

    // assert(utils::remove_n("12345", (float)2) == "1245"); // compile-time
    // assert((utils::remove_n(set<int>{1,2,3}, (float)2.5) == set<int>{1,2})); // compile-time
    // assert(utils::remove("12345", (long)3) == "1245"); // compile-time error
    // assert(utils::push_front("12345", 6) == "612345"); // compile-time error
    // assert(utils::push_back("12345", 6) == "123456"); // compile-time error
    // assert(utils::insert("12345", 2, (long)6) == "126345"); // compile-time error
    // assert((utils::insert(set<int>{1,2,3}, 4, '1') == set<int>{1,111,2,3}));// compile-time error
    // assert((utils::push_back(set<int>{1,2,3}, '4') == set<int>{1,2,3,4})); // compile-time
    // assert((utils::push_front(set<int>{1,2,3}, '4') == set<int>{1,2,3,4})); // compile-time

    {
        std::string s {"12345"};
        utils::nocopy::remove(s, '3');
        assert(s == "1245");
    }

    assert(utils::to_string(1) == "1");

    struct Foo { std::string to_string() const { return "Foo"; } };
    struct Bar { std::string toString() const { return "Bar"; } };

    assert(utils::to_string(Foo{}) == "Foo");
    assert(utils::to_string(Bar{}) == "Bar");
    assert(utils::to_string(Dar{}) == "Dar");

    const vector<vector<int>> vv = {{ {1,2,3}, {4,5,6} } };

    assert(utils::join(vector<int>{1,2,3}, " ") == "1 2 3");
    assert(utils::join(vector<int>{1,2,3}) == "1,2,3");
    assert(utils::join(utils::join(vv)) == "1,2,3,4,5,6");
    assert(utils::join(utils::split("a,b,c", ","), " ") == "a b c");
    assert(utils::join(std::string{"123"}) == "123");
    assert(utils::join("123") == "123");

    assert(utils::trim("   asd  ") == "asd");
    assert(utils::trim("   asd") == "asd");
    assert(utils::trim("asd  ") == "asd");
    assert(utils::trim("asd") == "asd");
    assert(utils::ltrim("  asd  ") == "asd  ");
    assert(utils::ltrim("asd  ") == "asd  ");
    assert(utils::rtrim("  asd  ") == "  asd");
    assert(utils::rtrim("asd  ") == "asd");

    assert(utils::combine(std::string("asd "), std::string("fgh")) == "asd fgh");
    assert(utils::combine(std::string("asd "), "fgh") == "asd fgh");
    assert(utils::combine("asd ", "fgh") == "asd fgh");

    assert(utils::to_lower("aSdDUnnbD") == "asddunnbd");
    assert(utils::to_upper("aSdDUnnbD") == "ASDDUNNBD");

    assert(utils::contains("aSdDUnnbD", "nnb"));
    assert(utils::contains(std::string{"aSdDUnnbD"}, "nnb", 'n'));
    assert(utils::contains("aSdDUnnbD", "nnb", 'n'));
    assert(!utils::contains("aSdDUnnbD", "nnb", 'n', 'z'));
    assert(utils::contains("aSdDUnnbD", "nnb", 'n', vector<char>{'n','n','b','D'}));
    assert(utils::starts_with("aSdDUnnbD", "aSd"));
    assert(utils::ends_with("aSdDUnnbDx", "nbDx"));

    assert(utils::capitalize("aSdDUnnbDx") == "Asddunnbdx");
    assert(utils::capitalize(std::string{"aSdDUnnbDx"}) == "Asddunnbdx");

    utils::nocopy::foreach([](const int& i){}, v1);

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::take(t, 3);
        assert((t == vector<int>{1,2,3}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::take_while(t, [](const int& i){return i<=3;});
        assert((t == vector<int>{1,2,3}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::drop(t, 3);
        assert((t == vector<int>{5,6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::drop_while(t, [](const int& i){return i<=3;});
        assert((t == vector<int>{5,6}));
    }

    assert(utils::take("12356", 3) == "123");
    assert(utils::drop("12356", 3) == "56");
    assert(utils::take_while("12356", [](const char i){return i<='3';}) == "123");
    assert(utils::drop_while("12356", [](const char i){return i<='3';}) == "56");
    assert(utils::drop_while("12356", [](const char i){return i>'3';}) == "12356");
    assert(utils::take_while("12356", [](const char i){return i>'3';}) == "");

    assert((utils::take(vector<int>{1,2,3,5,6}, 3) == vector<int>{1,2,3}));
    assert((utils::take_while(vector<int>{1,2,3,5,6}, [](const int& i){return i<=3;}) ==
            vector<int>{1,2,3}));
    assert((utils::take_while(vector<int>{1,2,3,5,6}, [](const int& i){return i>3;}) ==
            vector<int>{}));

    assert((utils::drop(vector<int>{1,2,3,5,6}, 3) == vector<int>{5,6}));
    assert((utils::drop_while(vector<int>{1,2,3,5,6}, [](const int& i){return i<=3;}) ==
            vector<int>{5,6}));
    assert((utils::drop_while(vector<int>{1,2,3,5,6}, [](const int& i){return i>3;}) ==
            vector<int>{1,2,3,5,6}));

    assert((utils::slice(vector<int>{1,2,3,5,6}) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, 0) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, 3) == vector<int>{5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3) == vector<int>{3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -1) == vector<int>{6}));
    assert((utils::slice(set<int>{1,2,3,5,6}, -1) == set<int>{6}));

    assert((utils::slice(vector<int>{1,2,3,5,6}, 2, 3) == vector<int>{3}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 3) == vector<int>{3}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 4) == vector<int>{3,5}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 5) == vector<int>{3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 6) == vector<int>{3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -4) == vector<int>{}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -3) == vector<int>{}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -2) == vector<int>{3}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -1) == vector<int>{3,5}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -5, -1) == vector<int>{1,2,3,5}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -5) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -10) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -10, 50) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{}, -5) == vector<int>{}));
    assert((utils::slice(set<int>{1,2,3,5,6}, -4, -1) == set<int>{2,3,5}));

    assert(utils::slice("12345") == "12345");
    assert(utils::slice("12345", 0, 4) == "1234");
    assert(utils::slice("12345", 0, 5) == "12345");
    assert(utils::slice("12345", 0, 60) == "12345");
    assert(utils::slice("12345", -60, 60) == "12345");
    assert(utils::slice("12345", -4, -1) == "234");
    assert(utils::slice(std::string{"12345"}, -4, -1) == "234");

    assert((utils::tail(vector<int>{1,2,3,4}) == vector<int>{2,3,4}));

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::tail(t);
        assert((t == vector<int>{2,3,5,6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 5);
        assert((t == vector<int>{}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 4);
        assert((t == vector<int>{6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 3);
        assert((t == vector<int>{5,6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 2, 3);
        assert((t == vector<int>{3}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, -3, -1);
        assert((t == vector<int>{3,5}));
    }


    assert(utils::size(set<int>{1,2}) == 2);
    assert(utils::size(set<int>{1,2}, set<int>{1,2,3}) == 5);
    //assert(utils::size(set<int>{1,2}, set<int>{1,2,3}, 1) == 5); // compile-time error


    assert(!utils::empty(set<int>{1,2}));
    assert(!utils::empty(set<int>{1,2}, set<int>{1,2,3}));
    assert(utils::empty(set<int>{1,2}, set<int>{1,2,3}, set<int>{}));
    assert(utils::max(1,2,3,4) == 4);
    assert(utils::max(8,1,2,3,4) == 8);
    assert(utils::max(8,1) == 8);
    assert(utils::max(8) == 8);
    assert(utils::min(1,2,3,4) == 1);
    assert(utils::min(8,1,2,3,4) == 1);
    assert(utils::min(8,1) == 1);
    assert(utils::min(8) == 8);

    return 0;
}
