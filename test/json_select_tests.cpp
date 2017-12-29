
#include <string>

#include "thirdparty/microtest/microtest.h"
#include "sql_asserts.h"
#include "SQLParser.h"
#include "util/jsonprinter.h"
#include "util/sqlhelper.h"

using namespace hsql;

TEST(JsonSelectTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT * FROM students;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, false);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_TRUE(root.isMember("selectList"));
  ASSERT_TRUE(root.isMember("fromTable"));
  ASSERT_FALSE(root.isMember("whereClause"));
  ASSERT_FALSE(root.isMember("groupBy"));

}

TEST(JsonSelectExprTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT a, MAX(b), CUSTOM(c, F(un)) FROM students;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, false);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_FALSE(root.isMember("whereClause"));
  ASSERT_FALSE(root.isMember("groupBy"));

  ASSERT_EQ(root["selectList"].size(), 3);

  ASSERT_STREQ(root["selectList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["selectList"][0]["name"].asString(), "a");

  ASSERT_STREQ(root["selectList"][1]["type"].asString(), "kExprFunctionRef");
  ASSERT_STREQ(root["selectList"][1]["name"].asString(), "MAX");
  ASSERT_EQ(root["selectList"][1]["exprList"].size(), 1);
  ASSERT_STREQ(root["selectList"][1]["exprList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["selectList"][1]["exprList"][0]["name"].asString(), "b");

  ASSERT_STREQ(root["selectList"][2]["type"].asString(), "kExprFunctionRef");
  ASSERT_STREQ(root["selectList"][2]["name"].asString(), "CUSTOM");
  ASSERT_EQ(root["selectList"][2]["exprList"].size(), 2);
  ASSERT_STREQ(root["selectList"][2]["exprList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["selectList"][2]["exprList"][0]["name"].asString(), "c");
  ASSERT_STREQ(root["selectList"][2]["exprList"][1]["type"].asString(), "kExprFunctionRef");
  ASSERT_STREQ(root["selectList"][2]["exprList"][1]["name"].asString(), "F");
  ASSERT_EQ(root["selectList"][2]["exprList"][1]["exprList"].size(), 1);
  ASSERT_STREQ(root["selectList"][2]["exprList"][1]["exprList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["selectList"][2]["exprList"][1]["exprList"][0]["name"].asString(), "un");

}


TEST(JsonSelectHavingTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT city, AVG(grade) AS avg_grade FROM students GROUP BY city HAVING AVG(grade) < -2.0",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_FALSE(root["selectDistinct"].asBool());

  ASSERT_TRUE(root.isMember("groupBy"));
  ASSERT_EQ(root["groupBy"]["columns"].size(), 1);
  ASSERT_STREQ(root["groupBy"]["having"]["opType"].asString(), "kOpLess");

  ASSERT_STREQ(root["groupBy"]["having"]["exprList"][0]["type"].asString(), "kExprFunctionRef");
  ASSERT_STREQ(root["groupBy"]["having"]["exprList"][1]["type"].asString(), "kExprLiteralFloat");
  ASSERT_EQ(root["groupBy"]["having"]["exprList"][1]["fval"].asFloat(), -2.0);
}


TEST(JsonSelectDistinctTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT DISTINCT grade, city FROM students;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_TRUE(root["selectDistinct"].asBool());
  ASSERT_FALSE(root.isMember("whereClause"));
}

TEST(JsonSelectSchemaTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT grade FROM some_schema.students;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT(root.isMember("fromTable"));
  ASSERT_STREQ(root["fromTable"]["schema"].asString(), "some_schema")
}

TEST(JsonSelectGroupDistinctTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT city, COUNT(name), COUNT(DISTINCT grade) FROM students GROUP BY city;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_FALSE(root["selectDistinct"].asBool());
  ASSERT_EQ(root["selectList"].size(), 3);
  ASSERT_FALSE(root["selectList"][1]["distinct"].asBool());
  ASSERT_TRUE(root["selectList"][2]["distinct"].asBool());
}

TEST(JsonOrderByTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT grade, city FROM students ORDER BY grade, city DESC;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_FALSE(root.isMember("whereClause"));
  ASSERT(root.isMember("order"));

  ASSERT_EQ(root["order"].size(), 2);
  ASSERT(root["order"][0]["ascending"].asBool());
  ASSERT_STREQ(root["order"][0]["name"].asString(), "grade")

  ASSERT_FALSE(root["order"][1]["ascending"].asBool());
  ASSERT_STREQ(root["order"][1]["name"].asString(), "city")
}

TEST(JsonSelectBetweenTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT grade, city FROM students WHERE grade BETWEEN -1 and c;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT(root.isMember("whereClause"));
  ASSERT_STREQ(root["whereClause"]["type"].asString(), "kExprOperator");
  ASSERT_STREQ(root["whereClause"]["opType"].asString(), "kOpBetween");

  ASSERT_STREQ(root["whereClause"]["exprList"][0]["name"].asString(), "grade");
  ASSERT_STREQ(root["whereClause"]["exprList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_EQ(root["whereClause"]["exprList"].size(), 3);

  ASSERT_STREQ(root["whereClause"]["exprList"][1]["type"].asString(), "kExprLiteralInt");
  ASSERT_EQ(root["whereClause"]["exprList"][1]["ival"].asInt(), -1);

  ASSERT_STREQ(root["whereClause"]["exprList"][2]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["whereClause"]["exprList"][2]["name"].asString(), "c");
}

TEST(JsonSelectConditionalSelectTest) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT * FROM t WHERE a = (SELECT MIN(v) FROM tt) AND EXISTS (SELECT * FROM test WHERE x < a);",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT(root.isMember("whereClause"));
  ASSERT_STREQ(root["whereClause"]["type"].asString(), "kExprOperator");
  ASSERT_STREQ(root["whereClause"]["opType"].asString(), "kOpAnd");

  ASSERT(root["whereClause"].isMember("exprList"));
  ASSERT_EQ(root["whereClause"]["exprList"].size(), 2);

  // a = (SELECT ...)
  ASSERT_STREQ(root["whereClause"]["exprList"][0]["opType"].asString(), "kOpEquals");
  ASSERT_STREQ(root["whereClause"]["exprList"][0]["exprList"][0]["name"].asString(), "a");
  ASSERT_STREQ(root["whereClause"]["exprList"][0]["exprList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["whereClause"]["exprList"][0]["exprList"][1]["type"].asString(), "kExprSelect");
  ASSERT(root["whereClause"]["exprList"][0]["exprList"][1].isMember("select"));
  ASSERT_STREQ(root["whereClause"]["exprList"][0]["exprList"][1]["select"]["fromTable"]["name"].asString(), "tt");

  // EXISTS (SELECT ...)
  ASSERT_STREQ(root["whereClause"]["exprList"][1]["opType"].asString(), "kOpExists");
  ASSERT_STREQ(root["whereClause"]["exprList"][1]["select"]["fromTable"]["name"].asString(), "test");

}

TEST(JsonSelectCaseWhen) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT MAX(CASE WHEN a = 'foo' THEN x ELSE 0 END) FROM test;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_EQ(root["selectList"].size(), 1);
  ASSERT_STREQ(root["selectList"][0]["type"].asString(), "kExprFunctionRef");
  ASSERT_EQ(root["selectList"][0]["exprList"].size(), 1);
  ASSERT_STREQ(root["selectList"][0]["exprList"][0]["type"].asString(), "kExprOperator");
  ASSERT_STREQ(root["selectList"][0]["exprList"][0]["opType"].asString(), "kOpCase");
  ASSERT_STREQ(root["selectList"][0]["exprList"][0]["exprList"][0]["type"].asString(), "kExprOperator");
  ASSERT_STREQ(root["selectList"][0]["exprList"][0]["exprList"][0]["opType"].asString(), "kOpEquals");
  ASSERT_EQ(root["selectList"][0]["exprList"][0]["exprList"].size(), 3);
}

TEST(JsonSelectJoin) {
  TEST_PARSE_SINGLE_SQL(
    "SELECT City.name, Product.category, SUM(price) FROM fact\
	  INNER JOIN City ON fact.city_id = City.id\
	  OUTER JOIN Product ON fact.product_id = Product.id\
	  GROUP BY City.name, Product.category;",
    kStmtSelect,
    SelectStatement,
    result,
    stmt);

  Json::Value root;
  Json::Reader reader;
  std::string json = jsonPrintStatementInfo(stmt, true);
  ASSERT_TRUE(reader.parse(json, root));

  ASSERT_STREQ(root["fromTable"]["type"].asString(), "kTableJoin");
  ASSERT_STREQ(root["fromTable"]["join"]["type"].asString(), "kJoinOuter");
  ASSERT_STREQ(root["fromTable"]["join"]["right"]["type"].asString(), "kTableName");
  ASSERT_STREQ(root["fromTable"]["join"]["right"]["name"].asString(), "Product");
  ASSERT_STREQ(root["fromTable"]["join"]["condition"]["opType"].asString(), "kOpEquals");

  ASSERT_STREQ(root["fromTable"]["join"]["condition"]["exprList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["fromTable"]["join"]["condition"]["exprList"][0]["table"].asString(), "fact");
  ASSERT_STREQ(root["fromTable"]["join"]["condition"]["exprList"][0]["name"].asString(), "product_id");

  ASSERT_STREQ(root["fromTable"]["join"]["condition"]["exprList"][1]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["fromTable"]["join"]["condition"]["exprList"][1]["table"].asString(), "Product");
  ASSERT_STREQ(root["fromTable"]["join"]["condition"]["exprList"][1]["name"].asString(), "id");

  // Joins are are left associative.
  // So the second join should be on the left.
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["type"].asString(), "kTableJoin");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["type"].asString(), "kJoinInner");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["left"]["type"].asString(), "kTableName");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["left"]["name"].asString(), "fact");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["right"]["type"].asString(), "kTableName");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["right"]["name"].asString(), "City");

  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["condition"]["exprList"][0]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["condition"]["exprList"][0]["table"].asString(), "fact");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["condition"]["exprList"][0]["name"].asString(), "city_id");

  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["condition"]["exprList"][1]["type"].asString(), "kExprColumnRef");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["condition"]["exprList"][1]["table"].asString(), "City");
  ASSERT_STREQ(root["fromTable"]["join"]["left"]["join"]["condition"]["exprList"][1]["name"].asString(), "id");
}
