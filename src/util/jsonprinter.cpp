#include "jsonprinter.h"

#include <string>
#include <iostream>

namespace hsql {

  void jsonPrintOperatorExpression(Expr* expr, Json::Value& root);

  std::string joinEnumToString(JoinType type) {

    switch (type) {
    case kJoinInner:
      return  "kJoinInner";
    case kJoinOuter:
      return "kJoinOuter";
    case kJoinLeft:
      return  "kJoinLeft";
    case kJoinRight:
      return  "kJoinRight";
    case kJoinLeftOuter:
      return  "kJoinLeftOuter";
    case kJoinRightOuter:
      return  "kJoinRightOuter";
    case kJoinCross:
      return  "kJoinCross";
    case kJoinNatural:
      return  "kJoinNatural";
    default:
      std::cerr << "Unrecognized join type " << type << std::endl;
      return NULL;
    }
  }

  void jsonPrintTableRefInfo(TableRef* table, Json::Value& root) {
    switch (table->type) {
    case kTableName:
      root["type"] = "kTableName";
      root["name"] = table->name;
      if(table->schema)  {
        root["schema"] = table->schema;
      }
      break;
    case kTableSelect:
      root["type"] = "kTableSelect";
      jsonPrintSelectStatementInfo(table->select, root["select"]);
      break;
    case kTableJoin:
      root["type"] = "kTableJoin";
      root["join"]["type"] = joinEnumToString(table->join->type);
      jsonPrintTableRefInfo(table->join->left, root["join"]["left"]);
      jsonPrintTableRefInfo(table->join->right, root["join"]["right"]);
      jsonPrintExpression(table->join->condition, root["join"]["condition"]);
      break;
    case kTableCrossProduct:
      root["type"] = "kTableCrossProduct";
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
    case kOpNone:
      root["opType"] = "kOpNone";
      break;
    case kOpBetween:
      root["opType"] = "kOpBetween";
      break;
    case kOpCase:
      root["opType"] = "kOpCase";
      break;
    case kOpPlus:
      root["opType"] = "kOpPlus";
      break;
    case kOpMinus:
      root["opType"] = "kOpMinus";
      break;
    case kOpAsterisk:
      root["opType"] = "kOpAsterisk";
      break;
    case kOpSlash:
      root["opType"] = "kOpSlash";
      break;
    case kOpPercentage:
      root["opType"] = "kOpPercentage";
      break;
    case kOpCaret:
      root["opType"] = "kOpCaret";
      break;
    case kOpEquals:
      root["opType"] = "kOpEquals";
      break;
    case kOpNotEquals:
      root["opType"] = "kOpNotEquals";
      break;
    case kOpLess:
      root["opType"] = "kOpLess";
      break;
    case kOpLessEq:
      root["opType"] = "kOpLessEq";
      break;
    case kOpGreater:
      root["opType"] = "kOpGreater";
      break;
    case kOpGreaterEq:
      root["opType"] = "kOpGreaterEq";
      break;
    case kOpLike:
      root["opType"] = "kOpLike";
      break;
    case kOpNotLike:
      root["opType"] = "kOpNotLike";
      break;
    case kOpILike:
      root["opType"] = "kOpILike";
      break;
    case kOpAnd:
      root["opType"] = "kOpAnd";
      break;
    case kOpOr:
      root["opType"] = "kOpOr";
      break;
    case kOpIn:
      root["opType"] = "kOpIn";
      break;
    case kOpConcat:
      root["opType"] = "kOpConcat";
      break;
    case kOpNot:
      root["opType"] = "kOpNot";
      break;
    case kOpUnaryMinus:
      root["opType"] = "kOpUnaryMinus";
      break;
    case kOpIsNull:
      root["opType"] = "kOpIsNull";
      break;
    case kOpExists:
      root["opType"] = "kOpExists";
      break;
    default:
      root["opType"] = expr->opType;

      break;
    }

    if (expr->exprList != nullptr) {
      for (Expr* e : *expr->exprList) {
        Json::Value exp;
        jsonPrintExpression(e, exp);
        root["exprList"].append(exp);
      }
    }

    if(expr->select != nullptr) {
      jsonPrintSelectStatementInfo(expr->select, root["select"]);
    }
  }

  void jsonPrintExpression(Expr* expr, Json::Value& root) {
    switch (expr->type) {
    case kExprStar:
      root["exp"] = "*";
      break;
    case kExprColumnRef:
      root["type"] = "kExprColumnRef";
      if(expr->table) {
        root["name"] = expr->name;
        root["table"] = expr->table;
      } else {
        root["name"] = expr->name;
      }
      break;
    case kExprLiteralFloat:
      root["type"] = "kExprLiteralFloat";
      root["fval"] = expr->fval;
      break;
    case kExprLiteralInt:
      root["type"] = "kExprLiteralInt";
      root["ival"] = (Json::Value::Int64)expr->ival;
      break;
    case kExprLiteralString:
      root["type"] = "kExprLiteralString";
      root["name"] = expr->name;
      break;
    case kExprFunctionRef:
      root["type"] = "kExprFunctionRef";
      root["distinct"] = expr->distinct;
      root["name"] = expr->name;
      for (Expr* e : *expr->exprList) {
        Json::Value exp;
        jsonPrintExpression(e, exp);
        root["exprList"].append(exp);
      }
      break;
    case kExprOperator:
      root["type"] = "kExprOperator";
      jsonPrintOperatorExpression(expr, root);
      break;
    case kExprSelect:
      root["type"] = "kExprSelect";
      jsonPrintSelectStatementInfo(expr->select, root["select"]);
      break;
    case kExprParameter:
      root["type"] = "kExprParameter";
      root["ival"] = (Json::Value::Int64)expr->ival;
      break;
    case kExprArray:
      root["type"] = "kExprArray";
      for (Expr* e : *expr->exprList) {
        Json::Value exp;
        jsonPrintExpression(e, exp);
        root["exprList"].append(exp);
      }
      break;
    case kExprArrayIndex:
      root["type"] = "kExprArrayIndex";
      // jsonPrintExpression(expr->expr, root["expr"]);
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
    root["selectDistinct"] = stmt->selectDistinct;
    for (Expr* expr : *stmt->selectList) {
      Json::Value val;
      jsonPrintExpression(expr, val);
      root["selectList"].append(val);
    }

    if (stmt->fromTable != nullptr) {
      jsonPrintTableRefInfo(stmt->fromTable, root["fromTable"]);
    }

    if (stmt->whereClause != nullptr) {
      jsonPrintExpression(stmt->whereClause, root["whereClause"]);
    }

    if (stmt->groupBy != nullptr) {
      for (Expr* expr : *stmt->groupBy->columns) {
        Json::Value val;
        jsonPrintExpression(expr, val);
        root["groupBy"]["columns"].append(val);
      }
      if (stmt->groupBy->having != nullptr) {
        jsonPrintExpression(stmt->groupBy->having, root["groupBy"]["having"]);
      }
    }

    if (stmt->unionSelect != nullptr) {
      jsonPrintSelectStatementInfo(stmt->unionSelect, root["unionSelect"]);
    }

    if (stmt->order != nullptr) {

      for (OrderDescription* order : *stmt->order) {
        Json::Value val;
        jsonPrintExpression(order->expr, val);
        if(order->type == kOrderAsc)
          val["ascending"] = true;
        else val["ascending"] = false;

        root["order"].append(val);
      }
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
