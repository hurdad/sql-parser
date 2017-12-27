#include "jsonprinter.h"

#include <string>
#include <iostream>

namespace hsql {

  void jsonPrintOperatorExpression(Expr* expr, Json::Value& root);

  void jsonPrintTableRefInfo(TableRef* table, Json::Value& root) {
    switch (table->type) {
    case kTableName:
      root["name"] = table->name;

      if(table->schema)  {
        root["schema"] = table->schema;
      }
      break;
    case kTableSelect:
      jsonPrintSelectStatementInfo(table->select, root["select"]);
      break;
    case kTableJoin:
      jsonPrintTableRefInfo(table->join->left, root["join_left"]);
      jsonPrintTableRefInfo(table->join->right, root["join_right"]);
      jsonPrintExpression(table->join->condition, root["join_condition"]);
      break;
    case kTableCrossProduct:
      for (TableRef* tbl : *table->list) {
        Json::Value t;
        jsonPrintTableRefInfo(tbl, t);
        root["table"].append(t);
      }
      break;
    }
    if (table->alias != nullptr) {
      root["alias"] = table->alias;
    }
  }

  void jsonPrintOperatorExpression(Expr* expr, Json::Value& root) {
    if (expr == nullptr) {
      root = "null";
      return;
    }

    switch (expr->opType) {
    case kOpAnd:
      root["type"] = "AND";
      break;
    case kOpOr:
      root["type"] = "OR";
      break;
    case kOpNot:
      root["type"] = "NOT";
      break;
    default:
      root["type"] = expr->opType;

      break;
    }
    jsonPrintExpression(expr->expr, root["expr"]);
    if (expr->expr2 != nullptr) {
      jsonPrintExpression(expr->expr2, root["expr2"]);
    } else if (expr->exprList != nullptr) {
      for (Expr* e : *expr->exprList) {
        Json::Value exp;
        jsonPrintExpression(e, exp);
        root["exp"].append(exp);
      }
    }
  }

  void jsonPrintExpression(Expr* expr, Json::Value& root) {
    switch (expr->type) {
    case kExprStar:
      root["exp"] = "*";
      break;
    case kExprColumnRef:

      if(expr->table) {
        root["name"] = expr->name;
        root["table"] = expr->table;
      } else {
        root["name"] = expr->name;
      }
      break;
    // case kExprTableColumnRef: inprint(expr->table, expr->name, numIndent); break;
    case kExprLiteralFloat:
      root["fval"] = expr->fval;
      break;
    case kExprLiteralInt:
      root["ival"] = (Json::Value::Int64)expr->ival;
      break;
    case kExprLiteralString:
      root["name"] = expr->name;
      break;
    case kExprFunctionRef:
      root["function"] = expr->name;
      for (Expr* e : *expr->exprList) {
        Json::Value exp;
        jsonPrintExpression(e, exp);
        root["param"].append(exp);
      }
      break;
    case kExprOperator:
      jsonPrintOperatorExpression(expr, root["operator"]);
      break;
    case kExprSelect:
      jsonPrintSelectStatementInfo(expr->select, root["select"]);
      break;
    case kExprParameter:
      root["ival"] = (Json::Value::Int64)expr->ival;
      break;
    case kExprArray:
      for (Expr* e : *expr->exprList) {
        Json::Value exp;
        jsonPrintExpression(e, exp);
        root["exp"].append(exp);
      }
      break;
    case kExprArrayIndex:
      jsonPrintExpression(expr->expr, root["exp"]);
      root["ival"] = (Json::Value::Int64)expr->ival;
      break;
    default:
      std::cerr << "Unrecognized expression type " << expr->type << std::endl;
      return;
    }
    if (expr->alias != nullptr) {
      root["alias"] = expr->alias;
    }
  }

  void jsonPrintSelectStatementInfo(const SelectStatement* stmt, Json::Value& root) {
    for (Expr* expr : *stmt->selectList) {
      Json::Value val;
      jsonPrintExpression(expr, val);
      root["fields"].append(val);
    }

    if (stmt->fromTable != nullptr) {
      jsonPrintTableRefInfo(stmt->fromTable, root["table"]);
    }

    if (stmt->whereClause != nullptr) {
      jsonPrintExpression(stmt->whereClause, root["where"]);
    }

    if (stmt->groupBy != nullptr) {
      for (Expr* expr : *stmt->groupBy->columns) {
        Json::Value val;
        jsonPrintExpression(expr, val);
        root["group_by"].append(val);

      }
      if (stmt->groupBy->having != nullptr) {
        jsonPrintExpression(stmt->groupBy->having, root["having"]);
      }
    }

    if (stmt->unionSelect != nullptr) {
      jsonPrintSelectStatementInfo(stmt->unionSelect, root["union"]);
    }

    if (stmt->order != nullptr) {
      jsonPrintExpression(stmt->order->at(0)->expr, root["orderby"]);
      if (stmt->order->at(0)->type == kOrderAsc) root["orderby_ascending"] = true;
      else root["orderby_ascending"] = false;
    }

    if (stmt->limit != nullptr) {
      root["limit"] = (Json::Value::Int64)stmt->limit->limit;
    }
  }


  void jsonPrintImportStatementInfo(const ImportStatement* stmt, Json::Value& root) {
    root["tableName"] = stmt->tableName;
    root["filePath"] = stmt->filePath;
  }

  void jsonPrintCreateStatementInfo(const CreateStatement* stmt, Json::Value& root) {
    root["tableName"] = stmt->tableName;
    root["filePath"] = stmt->filePath;
  }

  void jsonPrintInsertStatementInfo(const InsertStatement* stmt, Json::Value& root) {
    root["tableName"] = stmt->tableName;
    if (stmt->columns != nullptr) {

      Json::Value cols;
      for (char* col_name : *stmt->columns) {
        cols.append(col_name);
      }
      root["Columns"] = cols;
    }
    switch (stmt->type) {
    case kInsertValues:

      for (Expr* expr : *stmt->values) {
        Json::Value val;
        jsonPrintExpression(expr, val);
        root["values"].append(val);
      }
      break;
    case kInsertSelect:
      jsonPrintSelectStatementInfo(stmt->select, root);
      break;
    }
  }

  std::string jsonPrintStatementInfo(const SQLStatement* stmt, bool isStyled) {
    Json::Value root;
    switch (stmt->type()) {
    case kStmtSelect:
      jsonPrintSelectStatementInfo((const SelectStatement*) stmt, root);
      break;
    case kStmtInsert:
      jsonPrintInsertStatementInfo((const InsertStatement*) stmt, root);
      break;
    case kStmtCreate:
      jsonPrintCreateStatementInfo((const CreateStatement*) stmt, root);
      break;
    case kStmtImport:
      jsonPrintImportStatementInfo((const ImportStatement*) stmt, root);
      break;
    default:
      break;
    }

    if(isStyled) {
      Json::StyledWriter styledWriter;
      return styledWriter.write(root);
    } else {
      Json::FastWriter fastWriter;
      return fastWriter.write(root);
    }

  }

} // namespace hsql
