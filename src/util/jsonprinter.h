#ifndef __SQLPARSER__JSONPRINTER_H__
#define __SQLPARSER__JSONPRINTER_H__

#include <jsoncpp/json/json.h>

#include "../sql/statements.h"

namespace hsql {

  // Prints a summary of the given SQLStatement.
  std::string jsonPrintStatementInfo(const SQLStatement* stmt, bool isStyled);

  // Prints a summary of the given SelectStatement
  void jsonPrintSelectStatementInfo(const SelectStatement* stmt, Json::Value& root);

  // Prints a summary of the given ImportStatement
  void jsonPrintImportStatementInfo(const ImportStatement* stmt, Json::Value& root);

  // Prints a summary of the given InsertStatement
  void jsonPrintInsertStatementInfo(const InsertStatement* stmt, Json::Value& root);

  // Prints a summary of the given CreateStatement
  void jsonPrintCreateStatementInfo(const CreateStatement* stmt, Json::Value& root);

  // Prints a summary of the given Expression
  void jsonPrintExpression(Expr* expr, Json::Value& root);

} // namespace hsql

#endif
