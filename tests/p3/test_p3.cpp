// tests/p3/test_p3.cpp — YOUR P3 TEST SUITE.
//
// The five tests below are worked examples. Keep them, and add your own
// until you have AT LEAST 15 (spec §6 lists what must be covered).
// Name every test <area>_<what>: reg_ for the registry, ws_ for the
// workspace, calc_ for the calculator.
//
// Build & run:   cmake --build build && ./build/test_p3
// One area:      ./build/test_p3 reg_        (names starting with "reg_")
// One test:      ./build/test_p3 =reg_add_and_find

#include "../mini_test.h"
#include "../test_helpers.h"     // TempDir, write_text, args1, ...

#include "tools/calculator.h"
#include "tools/tool_registry.h"
#include "tools/workspace.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <iostream>

namespace fs = std::filesystem;

namespace {

// A tool whose name you choose — use it to fill the registry in tests.
class NamedTool : public Tool {
public:
    explicit NamedTool(std::string n) : n_(std::move(n)), schema_(JsonValue::object()) {}
    std::string name() const override { return n_; }
    std::string description() const override { return "test tool"; }
    const JsonValue& schema() const override { return schema_; }
    ToolResult invoke(const JsonValue&, ToolContext&) override { return ToolResult::success(n_); }
private:
    std::string n_;
    JsonValue   schema_;   // an empty JSON object: {}
};

// Tokenize with the provided tokenizer, then run YOUR calc_evaluate.
bool eval(const std::string& expr, double* value, std::string* error) {
    std::vector<CalcToken> tokens;
    if (!calc_tokenize(expr, &tokens, error)) return false;
    return calc_evaluate(tokens, value, error);
}

}  // namespace

// ---------------------------------------------------------------- examples

TEST(reg_add_and_find) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("alpha");
    Tool* raw = t.get();                        // remember the address before moving
    REQUIRE(r.add(std::move(t)));
    CHECK(r.size() == 1);
    CHECK(r.find("alpha") == raw);              // same object, not a copy
    CHECK(r.find("beta") == nullptr);
}

TEST(reg_duplicate_keeps_original) {
    ToolRegistry r;
    auto first = std::make_unique<NamedTool>("alpha");
    Tool* original = first.get();
    REQUIRE(r.add(std::move(first)));
    CHECK(!r.add(std::make_unique<NamedTool>("alpha")));
    CHECK(r.find("alpha") == original);
    CHECK(r.size() == 1);
}

TEST(ws_rejects_parent_escape) {
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;
    REQUIRE(ws.resolve("notes/todo.txt", &out, &reason));   // sanity: accepts *something*
    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(!ws.resolve("../etc/passwd", &out, &reason));
    CHECK(!reason.empty());
}

TEST(calc_precedence) {
    double v = 0;
    std::string err;
    REQUIRE(eval("2 + 3 * 4", &v, &err));
    CHECK(v == 14);
}

TEST(calc_division_by_zero) {
    double v = 0;
    std::string err;
    CHECK(!eval("1 / 0", &v, &err));
    CHECK(err == "division by zero");
}




// ------------------------------------------------------------- your tests
//
// Still to write (spec §6 has the full list). Suggested names:
//   reg_null_rejected, reg_for_each_visits_each_once
//   reg_10000_tools               REQUIRED: print bucket_count / load_factor /
//                                 max_probe_length, check the bound
//   reg_alternative_hash          REQUIRED: longest chain of a weak hash
//   ws_<case>                     REQUIRED: every row of the table in spec §4.4
//   calc_<case>                   left-to-right, parentheses, negatives

// NEED 15 OF THEM TESTS

TEST(reg_null_rejected){
    ToolRegistry r;
    REQUIRE(!r.add(nullptr));
}

TEST(reg_find_absent_name){
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("");
    Tool* raw = t.get();
    r.add(move(t));
    CHECK(r.find("") == raw);
}

TEST(reg_for_each_visits_each_once){
    ToolRegistry r;
    std::vector<std::string> list_names = {};
    REQUIRE(r.add(std::make_unique<NamedTool>("aaa")));
    REQUIRE(r.add(std::make_unique<NamedTool>("bbb")));
    REQUIRE(r.add(std::make_unique<NamedTool>("ccc")));
    r.for_each([&list_names](const Tool& tool){
        list_names.push_back(tool.name());
    });
    int aaa_count = 0; int bbb_count = 0; int ccc_count = 0;
    for(auto& iter : list_names){
        if(iter == "aaa"){aaa_count++;}
        if(iter == "bbb"){bbb_count++;}
        if(iter == "ccc"){ccc_count++;}
    }
    CHECK(aaa_count == 1 && bbb_count == 1 && ccc_count == 1);
}

TEST(reg_10000_tools){
    std::string str;
    ToolRegistry reg;
    char temp_i = 'a';
    char temp_j = 'a';
    char temp_k = 'a';
    Tool* the_pointer;
    // Make list of 10,000 tools, named "aaa", "aab", "aac", etc.
    // 25*25*16 = 10000, so these loop bounds should work
    for(int i = 0; i < 25; i++){
        str = temp_i;
        for(int j = 0; j < 25; j++){
            str.append({temp_j});
            for(int k = 0; k < 16; k++){
                str.append({temp_k});
                REQUIRE(reg.add(std::make_unique<NamedTool>(str)));
                CHECK(reg.load_factor() <= 1.0);
                CHECK(reg.max_probe_length() <= 64);
                temp_k++;
                // Create a pointer for the very first insert
                if(k == 0 && j == 0 && i == 0){
                    the_pointer = reg.find("aaa");
                }
            }
            temp_j++;
        }
        temp_i++;
    }
    // Check that the pointer is the same as it was before
    CHECK(reg.find("aaa") == the_pointer);
    
    CHECK(reg.size() == 10000);
    CHECK(std::cout << "Bucket Count: " << reg.bucket_count() << std::endl);
    CHECK(std::cout << "Load Factor: " << reg.load_factor() << std::endl);
    CHECK(std::cout << "Max Probe Length: " << reg.max_probe_length() << std::endl);

    // Checking if every single tool is findable with find()
    temp_i = 'a'; temp_j = 'a'; temp_k = 'a';
    for(int i = 0; i < 25; i++){
        str = temp_i;
        for(int j = 0; j < 25; j++){
            str.append({temp_j});
            for(int k = 0; k < 16; k++){
                str.append({temp_k});
                CHECK(reg.find(str) != nullptr);
                temp_k++;
            }
            temp_j++;
        }
        temp_i++;
    }


}

TEST(reg_10000_tools_weak_hash){
    std::string str;
    std::vector<int> counts(16384, 0);
    int amt_filled = 0;
    int highest_count = 0;

    char temp_i = 'a';
    char temp_j = 'a';
    char temp_k = 'a';
    // Make list of 10,000 tools, named "aaa", "aab", "aac", etc.
    // 25*25*16 = 10000, so these loop bounds should work
    for(int i = 0; i < 25; i++){
        str = temp_i;
        for(int j = 0; j < 25; j++){
            str.append({temp_j});
            for(int k = 0; k < 16; k++){
                str.append({temp_k});

                // Alternative hashing method.
                int terrible_hash = str[0] + str.size();
                counts[terrible_hash]++;
                if(counts[terrible_hash] == 1){
                    amt_filled++;
                }
                if(counts[terrible_hash] > highest_count){
                    highest_count = counts[terrible_hash];
                }

                temp_k++;
            }
            temp_j++;
        }
        temp_i++;
    }
    
    CHECK(std::cout << "Weak Hash Bucket Count: " << 16384 << std::endl);
    CHECK(std::cout << "Weak Hash Load Factor: " << static_cast<double>(amt_filled)/16384.0 << std::endl);
    CHECK(std::cout << "Weak Hash Max Probe Length: " << highest_count << std::endl);
}

// TODO: Maybe split these tables up into their cases but naming them seems silly
TEST(ws_table_pass_cases){
    // Some code shamelessly copied from the pre-provided test 'ws_rejects_parent_escape'
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;

    REQUIRE(ws.resolve("notes/todo.txt", &out, &reason));
    CHECK(out == ws.root() / "notes" / "todo.txt");

    REQUIRE(ws.resolve("./notes//todo.txt", &out, &reason));
    CHECK(out == ws.root() / "notes" / "todo.txt");

    REQUIRE(ws.resolve(".", &out, &reason));
    CHECK(out == ws.root());

    REQUIRE(ws.resolve("a/../b", &out, &reason));
    CHECK(out == ws.root() / "b");

    REQUIRE(ws.resolve("a/..", &out, &reason));
    CHECK(out == ws.root());

    REQUIRE(ws.resolve("new/dir/file.txt", &out, &reason));
    CHECK(out == ws.root() / "new" / "dir" / "file.txt");

    REQUIRE(ws.resolve("my..notes.txt", &out, &reason));
    CHECK(out == ws.root() / "my..notes.txt");
}

TEST(ws_table_fail_cases){
    // Some code shamelessly copied from the pre-provided test 'ws_rejects_parent_escape'
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;

    REQUIRE(!ws.resolve("", &out, &reason));
    CHECK(!reason.empty());

    REQUIRE(!ws.resolve(std::string("notes") + '\0' + "/x", &out, &reason));
    CHECK(!reason.empty());

    REQUIRE(!ws.resolve("/etc/passwd", &out, &reason));
    CHECK(!reason.empty());

    REQUIRE(!ws.resolve("..", &out, &reason));
    CHECK(!reason.empty());

    REQUIRE(!ws.resolve("../etc/passwd", &out, &reason));
    CHECK(!reason.empty());

    REQUIRE(!ws.resolve("a/../../b", &out, &reason));
    CHECK(!reason.empty());

    REQUIRE(!ws.resolve("notes/../../ws/notes/todo.txt", &out, &reason));
    CHECK(!reason.empty());
}

TEST(calc_table_add_mult) {
    double v = 0;
    std::string err;
    REQUIRE(eval("2 + 3 * 4", &v, &err));
    CHECK(v == 14);
}

TEST(calc_table_minus_twice) {
    double v = 0;
    std::string err;
    REQUIRE(eval("10 - 4 - 3", &v, &err));
    CHECK(v == 3);
}

TEST(calc_table_divide_twice){
    double v = 0;
    std::string err;
    REQUIRE(eval("100 / 10 / 5", &v, &err));
    CHECK(v == 2);
}

TEST(calc_table_starting_parenthesis) {
    double v = 0;
    std::string err;
    REQUIRE(eval("(2 + 3) * 4", &v, &err));
    CHECK(v == 20);
}

TEST(calc_table_order_of_operations){
    double v = 0;
    std::string err;
    REQUIRE(eval("2 * (3 + 4) - 5", &v, &err));
    CHECK(v == 9);
}

TEST(calc_table_double_negative){
    double v = 0;
    std::string err;
    REQUIRE(eval("-3 * -3", &v, &err));
    CHECK(v == 9);
}

TEST(calc_table_decimal){
    double v = 0;
    std::string err;
    REQUIRE(eval("7 / 2", &v, &err));
    CHECK(v == 3.5);
}

TEST(calc_table_div_by_parenthetical_zero){
    double v = 0;
    std::string err;
    CHECK(!eval("1 / (2 - 2)", &v, &err));
    CHECK(err == "division by zero");
}

TEST(calc_whole_lotta_numbers) {
    double v = 0;
    std::string err;
    REQUIRE(eval("( 2 + 3 * 9 ) / 8 + 16 - ( 2 * 8 + 2 )", &v, &err));
    CHECK(v == 1.625);
}

TEST(calc_nested_parentheticals) {
    double v = 0;
    std::string err;
    REQUIRE(eval("1 + ( 1 + ( 1 + ( 1 + ( 1 + ( 1 + ( 1 + ( 1 ) / 2 ) / 2 ) / 2 ) / 2 ) / 2 ) / 2 )", &v, &err));
    CHECK(v > 2.984 && v < 2.985);
}


int main(int argc, char** argv) { return mt::run_all(argc, argv); }
