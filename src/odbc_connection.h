/*
  Copyright (c) 2013, Dan VerWeire<dverweire@gmail.com>
  Copyright (c) 2010, Lee Smith<notwink@gmail.com>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef _SRC_ODBC_CONNECTION_H
#define _SRC_ODBC_CONNECTION_H

class ODBCConnection : public node::ObjectWrap {
  public:
   static Persistent<String> OPTION_SQL;
   static Persistent<String> OPTION_PARAMS;
   static Persistent<String> OPTION_NORESULTS;
   static Persistent<FunctionTemplate> constructor_template;
   
   static void Init(v8::Handle<Object> target);
   
   void Free();
   
  protected:
    ODBCConnection() {};
    
    explicit ODBCConnection(HENV hENV, HDBC hDBC): 
      ObjectWrap(),
      m_hENV(hENV),
      m_hDBC(hDBC) {};
     
    ~ODBCConnection();

    //constructor
    static Handle<Value> New(const Arguments& args);

    //Property Getter/Setters
    static Handle<Value> ConnectedGetter(Local<String> property, const AccessorInfo &info);
    static Handle<Value> ConnectTimeoutGetter(Local<String> property, const AccessorInfo &info);
    static void ConnectTimeoutSetter(Local<String> property, Local<Value> value, const AccessorInfo &info);
    
    //async methods
    static Handle<Value> BeginTransaction(const Arguments& args);
    static void UV_BeginTransaction(uv_work_t* work_req);
    static void UV_AfterBeginTransaction(uv_work_t* work_req, int status);
    
    static Handle<Value> EndTransaction(const Arguments& args);
    static void UV_EndTransaction(uv_work_t* work_req);
    static void UV_AfterEndTransaction(uv_work_t* work_req, int status);
    
    static Handle<Value> Open(const Arguments& args);
    static void UV_Open(uv_work_t* work_req);
    static void UV_AfterOpen(uv_work_t* work_req, int status);

    static Handle<Value> Close(const Arguments& args);
    static void UV_Close(uv_work_t* work_req);
    static void UV_AfterClose(uv_work_t* work_req, int status);

    static Handle<Value> CreateStatement(const Arguments& args);
    static void UV_CreateStatement(uv_work_t* work_req);
    static void UV_AfterCreateStatement(uv_work_t* work_req, int status);

    static Handle<Value> Query(const Arguments& args);
    static void UV_Query(uv_work_t* req);
    static void UV_AfterQuery(uv_work_t* req, int status);

    static Handle<Value> Columns(const Arguments& args);
    static void UV_Columns(uv_work_t* req);
    
    static Handle<Value> Tables(const Arguments& args);
    static void UV_Tables(uv_work_t* req);
    
    //sync methods
    static Handle<Value> CloseSync(const Arguments& args);
    static Handle<Value> CreateStatementSync(const Arguments& args);
    static Handle<Value> OpenSync(const Arguments& args);
    static Handle<Value> QuerySync(const Arguments& args);
    static Handle<Value> BeginTransactionSync(const Arguments& args);
    static Handle<Value> EndTransactionSync(const Arguments& args);
    
    struct Fetch_Request {
      Persistent<Function> callback;
      ODBCConnection *objResult;
      SQLRETURN result;
    };
    
    ODBCConnection *self(void) { return this; }

  protected:
    HENV m_hENV;
    HDBC m_hDBC;
    SQLUSMALLINT canHaveMoreResults;
    bool connected;
    int statements;
    int connectTimeout;
};

struct create_statement_work_data {
  Persistent<Function> cb;
  ODBCConnection *conn;
  HSTMT hSTMT;
  int result;
};

struct query_work_data {
  Persistent<Function> cb;
  ODBCConnection *conn;
  HSTMT hSTMT;
  
  Parameter *params;
  int paramCount;
  int completionType;
  bool noResultObject;
  
  void *sql;
  void *catalog;
  void *schema;
  void *table;
  void *type;
  void *column;
  
  int sqlLen;
  int sqlSize;
  
  int result;
};

struct open_connection_work_data {
  Persistent<Function> cb;
  ODBCConnection *conn;
  int result;
  int connectionLength;
  void* connection;
};

struct close_connection_work_data {
  Persistent<Function> cb;
  ODBCConnection *conn;
  int result;
};

#endif
