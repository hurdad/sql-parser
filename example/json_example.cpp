
#include <stdlib.h>
#include <string>

// include the sql parser
#include "SQLParser.h"

// contains printing utilities
#include "util/jsonprinter.h"


int main(int argc, char* argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "Usage: ./json_example \"SELECT * FROM test;\"\n");
    return -1;
  }
  std::string query = argv[1];

  // parse a given query
  hsql::SQLParserResult result;
  hsql::SQLParser::parse(query, &result);

  // check whether the parsing was successful

  if (result.isValid()) {
    printf("Parsed successfully!\n");
    printf("Number of statements: %lu\n", result.size());

    for (auto i = 0u; i < result.size(); ++i) {
      // Print statement as json
      std::string json_message = hsql::jsonPrintStatementInfo(result.getStatement(i), true);
      printf(json_message.c_str());
    }
    return 0;
  } else {
    fprintf(stderr, "Given string is not a valid SQL query.\n");
    fprintf(stderr, "%s (L%d:%d)\n",
            result.errorMsg(),
            result.errorLine(),
            result.errorColumn());
    return -1;
  }
}
