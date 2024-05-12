# include <stdlib.h>
# include <stdio.h>
# include <string>
# include <string.h>
# include <iostream>
# include <vector>
# include <iomanip>

# define DEBUG 0

using namespace std;

enum TokenType {
  ID, NUM, 
  INT, FLOAT, CHAR, BOOL, STRING, VOID,
  IF, ELSE, WHILE, DO, RETURN, CIN, COUT,
  LP, RP, MLP, MRP, BLP, BRP, // (), [], {}
  ADD, SUB, MULT, DIV, REM,  // +, -, *, /, %  
  BITAND, BITOR, BITXOR, BITNOT,  // &, |, ^, !
  EQ, NEQ, LSS, LE, GTR, GE, LS, RS,  // ==, !=, <, <=, >, >=, <<, >>
  AND, OR, PE, ME, TE, DE, RE, PP, MM, // &&, ||, +=, -=, *=, /=, %=, ++, --
  SCLN, COMMA, QUE, COLON, ASSIGN, // ';', ',', '?', ':', '='
  ADDORSIGN, SUBORSIGN, // + & -
  INPUTBUFFERAPPEND, ENDINPUTBUFFERAPPEND,
  NONE, END_OF_FILE, COMMENT  // ï¿½ï¿½lï¿½ï¿½
};

struct Token {
  string tokenString;
  TokenType tokenType;
  int lineNum;
};

struct Variable {
  string name;
  float val;
  bool isInt;
};

struct Function {
  string functionName;
  string type;
  vector<Token> tokenList;
};

struct Declarators {
  string type;
  string name;
  int num;
};

bool VariableCompare( Variable a, Variable b ) {
  return a.name > b.name; // ­°§Ç±Æ¦C
} // VariableCompare()

bool FunctionCompare( Function a, Function b ) {
  return a.functionName > b.functionName;
} // FunctionCompare()

int StringToInt( string str ) {
  int res = 0;

  for ( int i = 0 ; i < str.length() ; i++ ) {
    res = res * 10 + ( str[i] - '0' );
  } // end for

  return res;
} // StringToInt()

string IntToString( int num ) {
  if ( num == 0 ) return "0";
  
  string res = "";
  bool neg = false;
  if ( num < 0 ) {
    neg = true;
    num = -num;
  } // end if
  
  while ( num != 0 ) {
    char ch = ( '0' +  ( num % 10 ) );
    res = ch + res;
    num = num / 10;
  } // end while
  
  if ( neg ) return ( '-' + res );
  else return res;
} // IntToString()

class Scanner {
  public:
  int mCurLine;
  bool GetInput() ;
  Token mToken;
  vector<Token> mTokenList;
  int GetNextChar() ;
  void LineInit() ;
  Token GetMemberToken() ;
  Token GetToken() ;
  void SkipErrorLine() ;
  void CurInit() ;
  void Init() ;
  void PrintTokens() ;
   
  private:
      
  char mCur;
  bool mGetToken;
  void TokenInit() ;
  void SkipWhiteSpace() ;
  void SkipLineComment() ;
  void SkipMultiLineComment() ;
  void GetID() ;
  void GetNumber() ;
  void GetDoubleQuote() ;
  void GetSingleQuote() ;
  void GetParentheses() ;
  void GetArithmeticOp() ;
  void GetBitOp() ;
  void GetCompareOp() ;
  void GetOtherSpecial() ;
  void CheckReservedWords() ;
  // void PrintAllTokens() ;
}; // Scanner

void Scanner::Init() {
  mCur = '\0';
} // Scanner::Init()

void Scanner::LineInit() {
  mCurLine = 0;
} // Scanner::LineInit()

void Scanner::CurInit() {
  if ( mCur == '\0' || mCur == '\n' || mCur == '\r' ) {
    GetNextChar();
  } // end if
} // Scanner::CurInit()

void Scanner::TokenInit() {
  mToken.tokenString = "";
  mToken.tokenType = NONE;
} // Scanner::TokenInit()
   
int Scanner::GetNextChar() {
  int n = scanf( "%c", &mCur );
  if ( n == 0 ) mCur = '\0';
  return n;
} // Scanner::GetNextChar()
   
void Scanner::SkipWhiteSpace() {
  int checkInput;
  if ( mCur == '\n' ) {
    mCurLine++;
  } // end if
  
  checkInput = GetNextChar();
   
  while ( checkInput != 0 &&  ( mCur == ' ' || mCur == '\t' || mCur == '\n' ) ) {
    if ( mCur == '\n' ) {
      mCurLine++;
    } // end if
         
    checkInput = GetNextChar();
         
    if ( checkInput == 0 ) {
      mCur = '\0';
    } // end if
  } // while
      
  return;
} // Scanner::SkipWhiteSpace()
   

void Scanner::SkipLineComment() {
  int checkInput;
   
  while ( checkInput != 0 && mCur != '\n' ) {
    checkInput = GetNextChar();
    if ( checkInput == 0 ) {
      mCur = '\0';
    } // end if
  } // while()

  return;
} // Scanner::SkipLineComment()
   
void Scanner::SkipMultiLineComment() {
  int checkInput;
  bool end = false;
  checkInput = GetNextChar();
      
  while ( checkInput != 0 && ( ! end ) ) {
    if ( mCur == '\n' ) {
      mCurLine++;
    } // end if
         
    if ( mCur == '*' ) {
      checkInput = GetNextChar();
      if ( checkInput != 0 && mCur == '/' ) {
        end = true;
      } // end if
    } // end if
         
    if ( ! end ) checkInput = GetNextChar();
  } // while()
  
  GetNextChar();
  return;
} // Scanner::SkipMultiLineComment()
   
Token Scanner::GetToken() {
  TokenInit();
  mGetToken = false;
  while ( ! mGetToken ) {
    mGetToken = true;
    if ( mCur == '\0' ) {
      mToken.tokenType = END_OF_FILE;
      return mToken;
    } // end if
  

      
    if ( mCur == ' ' || mCur == '\t' || mCur == '\n' || mCur == '\r' ) {
      SkipWhiteSpace();
    } // end if
  
    if ( mCur == '\0' ) {
      mToken.tokenType = END_OF_FILE;
      return mToken;
    } // end if
 
    mToken.lineNum = mCurLine;
  
    if ( ( mCur >= 'a' && mCur <= 'z' ) || ( mCur >= 'A' && mCur <= 'Z' ) ) {
      GetID();
      mToken.tokenType = ID;
    } // end if
    else if ( ( mCur >= '0' && mCur <= '9' ) || mCur == '.' ) {
      GetNumber();
      mToken.tokenType = NUM;
    } // end if
    else if ( mCur == '(' || mCur == ')' ) {
      GetParentheses();   
    } // end if
    else if ( mCur == '+' || mCur == '-' || mCur == '*' || mCur == '/' ) {
      GetArithmeticOp();
    } // end if
    else if ( mCur == '>' || mCur == '<' || mCur == '=' || mCur == ':' ) {
      GetCompareOp();
    } // end if
    else if ( mCur == ';' ) {
      mToken.tokenString += mCur;
      GetNextChar();
      mToken.tokenType = SCLN;
    } // end if
    else {
      // printf( "char undefined! : %c", mCur );
      mToken.tokenString = mToken.tokenString + mCur;
      mToken.lineNum = mCurLine;
      string errorMsg = "Unrecognized token with first char : '" ;
      errorMsg = errorMsg + mCur + "'";
      throw ( errorMsg );
    } // end else
  } // end while

  mTokenList.push_back( mToken );
  return mToken;
} // Scanner::GetToken()

void Scanner::GetID() {
  mToken.tokenString += mCur;
  int checkInput = GetNextChar();
  bool end = false;
   
  while ( ( ! end ) && ( ( mCur >= 'a' && mCur <= 'z' ) || 
                         ( mCur >= 'A' && mCur <= 'Z' ) || 
                         ( mCur >= '0' && mCur <= '9' ) || 
                         mCur == '_' )  ) {
            
    if ( checkInput == 0 ) {
      end = true;
    } // end if

    if ( ! end ) {
      mToken.tokenString += mCur;
      checkInput = GetNextChar();
    } // end if
    
  } // while()
   
} // Scanner::GetID()

void Scanner::GetNumber() {
  mToken.tokenString += mCur;
  bool end = false;
  bool hasPoint = false;
  if ( mCur == '.' ) hasPoint = true;
  int checkInput = GetNextChar();
      
  while ( ( ! end ) && ( ( mCur >= '0' && mCur <= '9' ) || mCur == '.' )  ) {
    if ( checkInput == 0 ) {
      end = true;
    } // end if

    if ( mCur == '.' && hasPoint ) end = true;

    if ( ! end ) {
      mToken.tokenString += mCur;
      if ( mCur == '.' ) hasPoint = true;
      checkInput = GetNextChar();
    } // end if

    

  } // while()
   
} // Scanner::GetNumber()

void Scanner::GetDoubleQuote() {
  mToken.tokenString += mCur;
  bool end = false;
   
  int checkInput = GetNextChar();
   
  while ( ( ! end ) && mCur != '\"' ) {
    if ( checkInput == 0 ) {
      end = true;
    } // end if
      
    if ( ! end ) {
      mToken.tokenString += mCur;
      checkInput = GetNextChar();
    } // end if
    
  } // while()
  
  mToken.tokenString += mCur;    
  checkInput = GetNextChar(); // skip the double quote
} // Scanner::GetDoubleQuote()
   
void Scanner::GetSingleQuote() {
  mToken.tokenString += mCur;
  bool end = false;
  int checkInput = GetNextChar();
   
  while ( ( ! end ) && mCur != '\'' ) {
    if ( checkInput == 0 ) {
      end = true;
    } // end if

    if ( ! end ) {
      mToken.tokenString += mCur;
      checkInput = GetNextChar();
    } // end if
    
  } // while()
      
  mToken.tokenString += mCur;    
  checkInput = GetNextChar(); // skip the single quote   
  return;   
} // Scanner::GetSingleQuote()
   
void Scanner::GetParentheses() {
  if ( mCur == '(' ) {
    mToken.tokenString += mCur;
    mToken.tokenType = LP;
  } // end if
  else if ( mCur == ')' ) {
    mToken.tokenString += mCur;
    mToken.tokenType = RP;
  } // end if
   
  GetNextChar();
  return;
} // Scanner::GetParentheses()
   
void Scanner::GetArithmeticOp() {
       
  int checkInput;
      
  if ( mCur == '+' ) {
    mToken.tokenString += mCur;
    GetNextChar();
    mToken.tokenType = ADD;
  } // end if
  else if ( mCur == '-' ) {
    mToken.tokenString += mCur;
    mToken.tokenType = SUB;
    GetNextChar();
  } // end if
  else if ( mCur == '*' ) {
    mToken.tokenString += mCur;
      
    checkInput = GetNextChar();
    
    mToken.tokenType = MULT;
      
  } // end if
  else if ( mCur == '/' ) {
    mToken.tokenString += mCur;
     
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = DIV;
      return;
    } // end if
    else if ( mCur == '/' ) { // single line comment
      SkipLineComment();
      mToken.tokenString = "";
      mGetToken = false;
      return;
    } // end if
    else {
      mToken.tokenType = DIV;
    } // else
      
    if ( mToken.tokenType != DIV ) GetNextChar();
  } // end if
   
  return;
} // Scanner::GetArithmeticOp()
   
void Scanner::GetBitOp() {
  mToken.tokenString += mCur;
      
  int checkInput;   
      
  if ( mCur == '^' ) {
    mToken.tokenType = BITXOR;
    GetNextChar();
  } // end if
  else if ( mCur == '&' ) {
    checkInput = GetNextChar();
    if ( checkInput == 0 ) {
      mToken.tokenType = BITAND;
      return;
    } // end if
    else if ( mCur == '&' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = AND;
      GetNextChar();
    } // end if
    else {
      mToken.tokenType = BITAND;
      GetNextChar();
    } // end else
  } // end if
  else if ( mCur == '|' ) {
    checkInput = GetNextChar();
 
    if ( checkInput == 0 ) {
      mToken.tokenType = BITOR;
      return;
    } // end if
    else if ( mCur == '|' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = OR;
      GetNextChar();
    } // end if
    else {
      mToken.tokenType = BITOR;
      GetNextChar();
    } // end else
  } // end if

  return;
} // Scanner::GetBitOp()
   
void Scanner::GetCompareOp() {
  mToken.tokenString += mCur;
  int checkInput;   
      
      
  if ( mCur == '>' ) {
    checkInput = GetNextChar();
         
    if ( checkInput == 0 ) {
      mToken.tokenType = GTR;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = GE;
      GetNextChar();
    } // end if
    else {
      mToken.tokenType = GTR;
    } // else
  } // end if
  else if ( mCur == '<' ) {
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = LSS;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = LE;
      GetNextChar();
    } // end if
    else if ( mCur == '>' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = NEQ;
      GetNextChar();
    } // end if
    else {
      mToken.tokenType = LSS;
    } // else
  } // end if
  else if ( mCur == '=' ) {
    GetNextChar();
    mToken.tokenType = EQ;
  }  // end if
  else if ( mCur == ':' ) {
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = BITNOT;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = ASSIGN;
      GetNextChar();
    } // end if
    else {
      string errorMsg = "Unrecognized token with first char : " ;
      errorMsg = errorMsg + "':'";
      if ( DEBUG ) cout << errorMsg << endl;
      throw ( errorMsg );
    } // end else
    
  } // end if
} // Scanner::GetCompareOp()
   
void Scanner::GetOtherSpecial() {
  mToken.tokenString += mCur;
      
  if ( mCur == ';' ) {
    mToken.tokenType = SCLN;
  } // end if
  else if ( mCur == ',' ) {
    mToken.tokenType = COMMA;
  } // end if
  else if ( mCur == '?' ) {
    mToken.tokenType = QUE;
  } // end if
  else if ( mCur == ':' ) {
    mToken.tokenType = COLON;
  } // end if
   
  GetNextChar();
} // Scanner::GetOtherSpecial()



Token Scanner::GetMemberToken() {
  return mToken;
} // Scanner::GetMemberToken()

void Scanner::SkipErrorLine() {
  mToken.tokenString = "";
  mToken.tokenType = NONE;
  mToken.lineNum = 1;
  while ( mCur != '\n' ) {
    if ( !GetNextChar() )  return;
  } // end while

  return;
} // end Scanner::SkipErrorLine()

void Scanner::PrintTokens() {
  for ( int i = 0 ; i < mTokenList.size() ; i++ ) {
    cout << mTokenList[i].tokenString << " ";
  } // end for

  cout << endl;
} // Scanner::PrintTokens()

class Parser {
  public:
  bool IsCommand() ;
  void Init() ;
  void GetFirstToken() ;
  Scanner mScanner;

  private:
  Token mToken;
  vector<Variable> mVariableList;
  int mTokenCount;
  bool mIsBoolExp;
  bool mIsInt;
  bool mCountAsFloat;

  void Command() ;
  void IDlessArithExpOrBexp() ;
  bool BooleanOperator() ;
  void NOTIDStartArithExpOrBexp() ;
  void NOTIDStartArithExp() ;
  void NOTIDStartTerm() ;
  void NOTIDStartFactor() ;
  void ArithExp() ;
  void Term() ;
  void Factor() ;

  void CommandOp() ;
  float IDlessArithExpOrBexpOp( string name ) ;
  float NOTIDStartArithExpOrBexpOp() ;
  float NOTIDStartArithExpOp() ;
  float NOTIDStartTermOp() ;
  float NOTIDStartFactorOp() ;
  float ArithExpOp() ;
  float TermOp() ;
  float FactorOp() ;

  void NewVariable( string name, float num ) ;
  void CheckVariable( string name ) ;
  float StringToFloat( string num ) ;
  float GetVariableValue( string name ) ;
  int VariableExist( string name ) ;
}; // Parser

void Parser::Init() {
  bool end = false;
  mVariableList.clear();
  mScanner.mCurLine = 1;
  mTokenCount = 0;
  mIsBoolExp = false;
  mIsInt = true;
  mCountAsFloat = false;
  mScanner.GetNextChar();

  while ( ! end ) {
    try {
      mScanner.GetToken();
      end = true;
      mScanner.mTokenList.clear();

      mToken = mScanner.mToken;
      mScanner.mTokenList.push_back( mToken );
    } catch ( string msg ) {

      cout << msg << endl;
      mScanner.LineInit();
      mScanner.SkipErrorLine();
      mScanner.mCurLine++;
      mScanner.GetNextChar();
    } // catch
  } // end while
} // Parser::Init()

void Parser::GetFirstToken() {
  bool end = false;
  mScanner.LineInit();

  while ( ! end ) {
    try {
      mScanner.GetToken();
      end = true;
      mScanner.mTokenList.clear();
      if ( mScanner.mToken.lineNum == 0 ) {
        mScanner.mToken.lineNum = 1;
        mScanner.mCurLine = 1;
      } // end if

      mToken = mScanner.mToken;
      mScanner.mTokenList.push_back( mToken );
    } catch ( string msg ) {
      if ( mScanner.mToken.lineNum == 0 ) {
        mScanner.mToken.lineNum = 1;
      } // end if

      cout << msg << endl;
      mScanner.LineInit();
      mScanner.SkipErrorLine();
      mScanner.mCurLine++;
      mScanner.GetNextChar();
    } // catch
  } // end while
} // Parser::GetFirstToken()

bool Parser::IsCommand() {
  try {
    Command();
  } catch ( string msg ) {
    
    if ( msg == "EOF" || msg == "Done" )
      return false;
    cout << msg << endl;

    printf( "> " );

    mScanner.SkipErrorLine();
    GetFirstToken();
    mIsBoolExp = false;
    mIsInt = true;
    mCountAsFloat = false;
    mTokenCount = 0;
    return true;
  } // catch

  // mScanner.PrintTokens();
  mScanner.mTokenList.clear();
  printf( "> " );
  GetFirstToken();
  
  return true;
} // Parser::IsCommand()

void Parser::Command() {
  if ( mToken.tokenType == END_OF_FILE ) {
    string errorMsg = "EOF";
    throw ( errorMsg );
  } // end if
  else if ( mToken.tokenString == "quit" ) {
    string errorMsg = "Done";
    throw ( errorMsg );
  } // end if
  else if ( mToken.tokenType == ID ) {
    string name = mToken.tokenString;
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else if ( mToken.tokenType == ASSIGN ) {
      mToken = mScanner.GetToken();

      if ( mToken.tokenType == END_OF_FILE ) {
        string errorMsg = "EOF";
        throw ( errorMsg );
      } // end if
      else if ( mToken.tokenType == ID ||
                mToken.tokenType == ADD ||
                mToken.tokenType == SUB ||
                mToken.tokenType == LP ||
                mToken.tokenType == NUM ) {
        ArithExp();
      } // end if
      else {
        string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
        throw ( errorMsg );
      } // end else
    } // end if
    else if ( mToken.tokenType == MULT ||
              mToken.tokenType == ADD ||
              mToken.tokenType == SUB ||
              mToken.tokenType == DIV ||
              BooleanOperator() ) {

      CheckVariable( name );
      IDlessArithExpOrBexp();
    } // end if

    if ( mToken.tokenType != SCLN ) {
      string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mToken.tokenType == ADD ||
            mToken.tokenType == SUB ||
            mToken.tokenType == LP ||
            mToken.tokenType == NUM ) {
    NOTIDStartArithExpOrBexp();
    if ( mToken.tokenType != SCLN ) {
      string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
      throw ( errorMsg );
    } // end if
  } // end if
  else {
    string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
    throw ( errorMsg );
  } // end else

  CommandOp();
  mIsBoolExp = false;
  mIsInt = true;
  mCountAsFloat = false;
  mTokenCount = 0;


} // Parser::Command()

void Parser::IDlessArithExpOrBexp() {
  while ( mToken.tokenType == ADD || 
          mToken.tokenType == SUB ||
          mToken.tokenType == MULT ||
          mToken.tokenType == DIV ) {

    if ( mToken.tokenType == ADD || mToken.tokenType == SUB ) {
      mToken = mScanner.GetToken();

      if ( mToken.tokenType == END_OF_FILE ) {
        string errorMsg = "EOF";
        throw ( errorMsg );
      } // end if
      else {
        Term();
      } // end else
    } // end if
    else if ( mToken.tokenType == MULT || mToken.tokenType == DIV ) {
      mToken = mScanner.GetToken();

      if ( mToken.tokenType == END_OF_FILE ) {
        string errorMsg = "EOF";
        throw ( errorMsg );
      } // end if
      else {
        Factor();
      } // end else
    } // end if
  } // end while

  if ( BooleanOperator() ) {
    mToken = mScanner.GetToken();

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else {
      ArithExp();
    } // end else
  } // end if

} // Parser::IDlessArithExpOrBexp()

bool Parser::BooleanOperator() {
  if ( mToken.tokenType == EQ || 
       mToken.tokenType == NEQ ||
       mToken.tokenType == GTR || 
       mToken.tokenType == LSS || 
       mToken.tokenType == GE || 
       mToken.tokenType == LE ) {
    return true;
  } // end if

  return false;
} // Parser::BooleanOperator()

void Parser::NOTIDStartArithExpOrBexp() {
  NOTIDStartArithExp();

  if ( BooleanOperator() ) {
    mToken = mScanner.GetToken();

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else {
      ArithExp();
    } // end else
  } // end if

} // Parser::NOTIDStartArithExpOrBexp()

void Parser::NOTIDStartArithExp() {
  NOTIDStartTerm();

  while ( mToken.tokenType == ADD || mToken.tokenType == SUB ) {
    mToken = mScanner.GetToken();

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else {
      Term();
    } // end else
  } // end while

} // Parser::NOTIDStartArithExp()

void Parser::NOTIDStartTerm() {
  NOTIDStartFactor();

  while ( mToken.tokenType == MULT || mToken.tokenType == DIV ) {
    mToken = mScanner.GetToken();

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else {
      ArithExp();
    } // end else
  } // end while

} // Parser::NOTIDStartTerm()

void Parser::NOTIDStartFactor() {
  if ( mToken.tokenType == LP ) {
    mToken = mScanner.GetToken();

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else {
      ArithExp();
    } // end else

    if ( mToken.tokenType != RP ) {
      string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
      throw ( errorMsg );
    } // end if
  } // end if
  else {
    if ( mToken.tokenType == ADD || mToken.tokenType == SUB ) {
      mToken = mScanner.GetToken();

      if ( mToken.tokenType == END_OF_FILE ) {
        string errorMsg = "EOF";
        throw ( errorMsg );
      } // end if
    } // end if

    if ( mToken.tokenType != NUM ) {
      string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
      throw ( errorMsg );
    } // end if
  } // end else

  mToken = mScanner.GetToken();

  if ( mToken.tokenType == END_OF_FILE ) {
    string errorMsg = "EOF";
    throw ( errorMsg );
  } // end if
} // Parser::NOTIDStartFactor()

void Parser::ArithExp() {
  Term();

  while ( mToken.tokenType == ADD || mToken.tokenType == SUB ) {
    mToken = mScanner.GetToken();

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if

    Term();

  } // end while

} // Parser::ArithExp()

void Parser::Term() {
  Factor();

  while ( mToken.tokenType == MULT || mToken.tokenType == DIV ) {
    mToken = mScanner.GetToken();

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if

    Factor();
  } // end while

} // Parser::Term()

void Parser::Factor() {
  if ( mToken.tokenType == LP ) {
    mToken = mScanner.GetToken();


    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else {
      ArithExp();
    } // end else

    if ( mToken.tokenType == END_OF_FILE ) {
      string errorMsg = "EOF";
      throw ( errorMsg );
    } // end if
    else if ( mToken.tokenType != RP ) {
      string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mToken.tokenType != ID ) {
    
    if ( mToken.tokenType == ADD || mToken.tokenType == SUB ) {
      mToken = mScanner.GetToken();

      if ( mToken.tokenType == END_OF_FILE ) {
        string errorMsg = "EOF";
        throw ( errorMsg );
      } // end if
    } // end if

    if ( mToken.tokenType != NUM ) {
      string errorMsg = "Unexpected token : '" + mToken.tokenString + "'";
      throw ( errorMsg );
    } // end if
  } // end if
  else {
    CheckVariable( mToken.tokenString );
  } // end else
  
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string errorMsg = "EOF";
    throw ( errorMsg );
  } // end if

} // Parser::Factor()

void Parser::CommandOp() {

  float result;

  if ( mScanner.mTokenList[mTokenCount].tokenType == ID ) {
    string name = mScanner.mTokenList[mTokenCount].tokenString;
    mTokenCount++;

    if ( mScanner.mTokenList[mTokenCount].tokenType == ASSIGN ) {
      mTokenCount++;
      result = ArithExpOp();
      NewVariable( name, result );
    } // end if
    else {
      result = IDlessArithExpOrBexpOp( name );
    } // end else
  } // end if
  else if ( mScanner.mTokenList[mTokenCount].tokenType == ADD ||
            mScanner.mTokenList[mTokenCount].tokenType == SUB ||
            mScanner.mTokenList[mTokenCount].tokenType == LP ||
            mScanner.mTokenList[mTokenCount].tokenType == NUM ) {
    result = NOTIDStartArithExpOrBexpOp();
  } // end if

  if ( mIsBoolExp ) {
    if ( result == 0.0 ) cout << "false" << endl;
    else cout << "true" << endl;
  } // end if
  else if ( mIsInt ) {
    cout << ( int ) result << endl;
  } // end if
  else {
    printf( "%.3f\n", result );
  } // end else

} // Parser::CommandOp()

float Parser::IDlessArithExpOrBexpOp( string name ) {
  float res = GetVariableValue( name );

  // bool intoWhile = false;

  while ( mScanner.mTokenList[mTokenCount].tokenType == ADD || 
          mScanner.mTokenList[mTokenCount].tokenType == SUB ||
          mScanner.mTokenList[mTokenCount].tokenType == MULT ||
          mScanner.mTokenList[mTokenCount].tokenType == DIV ) {

    // intoWhile = true;
    if ( mScanner.mTokenList[mTokenCount].tokenType == ADD ) {
      mTokenCount++;
      res = res + TermOp();
    } // end if
    else if ( mScanner.mTokenList[mTokenCount].tokenType == SUB ) {
      mTokenCount++;
      res = res - TermOp();
    } // end if
    else if ( mScanner.mTokenList[mTokenCount].tokenType == MULT ) {
      mTokenCount++;
      res = res * FactorOp();
    } // end if
    else if ( mScanner.mTokenList[mTokenCount].tokenType == DIV ) {
      mTokenCount++;
      float ans = FactorOp();
      if ( ans == 0 ) {
        string errorMsg = "Error";
        throw ( errorMsg );
      } // end if

      if ( ! mCountAsFloat ) {
        mCountAsFloat = false;
        res = ( int ) res / ( int ) ans;
      } // end if
      else res = res / ans;

      if ( res < 0.0001 ) res = 0;
    } // end if
  } // end while

  if ( mScanner.mTokenList[mTokenCount].tokenType == EQ || 
       mScanner.mTokenList[mTokenCount].tokenType == NEQ ||
       mScanner.mTokenList[mTokenCount].tokenType == GTR || 
       mScanner.mTokenList[mTokenCount].tokenType == LSS || 
       mScanner.mTokenList[mTokenCount].tokenType == GE || 
       mScanner.mTokenList[mTokenCount].tokenType == LE ) {

    string op = mScanner.mTokenList[mTokenCount].tokenString;
    mIsBoolExp = true;
    mTokenCount++;
      
    if ( op == "=" ) {
      return ( int ) ( ( res - ArithExpOp() ) * 1000 ) == 0.0;
    } // end if
    else if ( op == "<>" ) {
      return ( int ) ( ( res - ArithExpOp() ) * 1000 ) != 0.0;
    } // end if
    else if ( op == ">" ) {
      return ( float ) ( res - ArithExpOp() ) > 0.0001;
    } // end if
    else if ( op == ">=" ) {
      return ( float ) ( res - ArithExpOp() ) >= 0.0001;
    } // end if
    else if ( op == "<" ) {
      return ( float ) ( res - ArithExpOp() ) < 0.0001;
    } // end if
    else if ( op == "<=" ) {
      return ( float ) ( res - ArithExpOp() ) <= 0.0001;
    } // end if
  } // end if

  return res;
} // Parser::IDlessArithExpOrBexpOp()

float Parser::NOTIDStartArithExpOrBexpOp() {
  float res = NOTIDStartArithExpOp();

  if ( mScanner.mTokenList[mTokenCount].tokenType == EQ || 
       mScanner.mTokenList[mTokenCount].tokenType == NEQ ||
       mScanner.mTokenList[mTokenCount].tokenType == GTR || 
       mScanner.mTokenList[mTokenCount].tokenType == LSS || 
       mScanner.mTokenList[mTokenCount].tokenType == GE || 
       mScanner.mTokenList[mTokenCount].tokenType == LE ) {

    string op = mScanner.mTokenList[mTokenCount].tokenString;
    mIsBoolExp = true;
    mTokenCount++;
    
    float a = ArithExpOp();

    if ( op == "=" ) {
      return ( int ) ( ( res - a ) * 1000 ) == 0.0;
    } // end if
    else if ( op == "<>" ) {
      return ( int ) ( ( res - a ) * 1000 ) != 0.0;
    } // end if
    else if ( op == ">" ) {
      return ( float ) res - a > 0.0001;
    } // end if
    else if ( op == ">=" ) {
      return ( float ) res - a >= 0.0001;
    } // end if
    else if ( op == "<" ) {
      return ( float ) res - a < 0.0001;
    } // end if
    else if ( op == "<=" ) {
      return ( float ) res - a <= 0.0001;
    } // end if
  } // end if

  return res;
} // Parser::NOTIDStartArithExpOrBexpOp()

float Parser::NOTIDStartArithExpOp() {
  float res = NOTIDStartTermOp();

  while ( mScanner.mTokenList[mTokenCount].tokenType == ADD || 
          mScanner.mTokenList[mTokenCount].tokenType == SUB ) {
    string op = mScanner.mTokenList[mTokenCount].tokenString;
    mTokenCount++;

    if ( op == "+" ) {
      
      res = res + TermOp();
    } // end if
    else {
      res = res - TermOp();
    } // end else
  } // end while

  return res;
} // Parser::NOTIDStartArithExpOp()

float Parser::NOTIDStartTermOp() {
  float res = NOTIDStartFactorOp();

  while ( mScanner.mTokenList[mTokenCount].tokenType == MULT || 
          mScanner.mTokenList[mTokenCount].tokenType == DIV ) {
    string op = mScanner.mTokenList[mTokenCount].tokenString;
    mTokenCount++;

    if ( op == "*" ) {
      res = res * FactorOp();
    } // end if
    else {
      float ans = FactorOp();
      if ( ans == 0 ) {
        string errorMsg = "Error";
        throw ( errorMsg );
      } // end if

      if ( ! mCountAsFloat ) {
        mCountAsFloat = false;
        res = ( int ) res / ( int ) ans;
      } // end if
      else res = res / ans;

      if ( res > 0 && res < 0.0001 ) res = 0;
    } // end else
  } // end while

  return res;
} // Parser::NOTIDStartTermOp()

float Parser::NOTIDStartFactorOp() {
  float res = 0;

  if ( mScanner.mTokenList[mTokenCount].tokenType == LP ) {
    mTokenCount++;

    res = ArithExpOp();

    mTokenCount++; // skip RP '('
    return res;
  } // end if
  else {
    bool neg = false;

    if ( mScanner.mTokenList[mTokenCount].tokenType == ADD ||
         mScanner.mTokenList[mTokenCount].tokenType == SUB ) {
      if ( mScanner.mTokenList[mTokenCount].tokenType == SUB ) neg = true;
      
      mTokenCount++;
    } // end if

    res = StringToFloat( mScanner.mTokenList[mTokenCount].tokenString );

    if ( neg ) {
      res = -res;
    } // end if
  } // end else

  mTokenCount++;

  return res;
} // Parser::NOTIDStartFactorOp()

float Parser::ArithExpOp() {
  float res = TermOp();

  while ( mScanner.mTokenList[mTokenCount].tokenType == ADD || 
          mScanner.mTokenList[mTokenCount].tokenType == SUB ) {
    string op = mScanner.mTokenList[mTokenCount].tokenString;
    mTokenCount++;

    if ( op == "+" ) {
      res = res + TermOp();
    } // end if
    else {
      res = res - TermOp();
    } // end else
  } // end while

  return res;
} // Parser::ArithExpOp()

float Parser::TermOp() {

  float res = FactorOp();

  while ( mScanner.mTokenList[mTokenCount].tokenType == MULT || 
          mScanner.mTokenList[mTokenCount].tokenType == DIV ) {
    string op = mScanner.mTokenList[mTokenCount].tokenString;
    mTokenCount++;

    if ( op == "*" ) {
      res = res * FactorOp();
    } // end if
    else {
      float ans = FactorOp();
      if ( ans == 0 ) {
        string errorMsg = "Error";
        throw ( errorMsg );
      } // end if

      if ( ! mCountAsFloat ) {
        mCountAsFloat = false;
        res = ( int ) res / ( int ) ans;
      } // end if
      else res = res / ans;

      if ( res < 0.0001 ) res = 0;
    } // end else
  } // end while


  return res;
} // Parser::TermOp()

float Parser::FactorOp() {
  float res = 0;

  if ( mScanner.mTokenList[mTokenCount].tokenType == LP ) {
    mTokenCount++;

    res = ArithExpOp();

    mTokenCount++; // skip RP '('
    return res;
  } // end if
  else {
    bool neg = false;
    if ( mScanner.mTokenList[mTokenCount].tokenType == ID ) {
      res = GetVariableValue( mScanner.mTokenList[mTokenCount].tokenString );
      mTokenCount++;
      return res;
    } // end if

    if ( mScanner.mTokenList[mTokenCount].tokenType == ADD ||
         mScanner.mTokenList[mTokenCount].tokenType == SUB ) {
      if ( mScanner.mTokenList[mTokenCount].tokenType == SUB ) neg = true;
      
      mTokenCount++;
    } // end if

    res = StringToFloat( mScanner.mTokenList[mTokenCount].tokenString );

    if ( neg ) {
      res = -res;
    } // end if
  } // end else

  mTokenCount++;

  return res;

} // Parser::FactorOp()

void Parser::CheckVariable( string name ) {
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( name == mVariableList[i].name ) {
      return;
    } // end if
  } // end for

  string errorMsg = "Undefined identifier : '" + name + "'";
  throw ( errorMsg );
}  // Parser::CheckVariable() 

void Parser::NewVariable( string name, float num ) {
  Variable temp;

  int findNum = VariableExist( name );

  if ( findNum >= 0 ) {
    mVariableList[findNum].val = num;
    mVariableList[findNum].isInt = mIsInt;
    return;
  } // end if

  temp.name = name;
  temp.val = num;
  temp.isInt = mIsInt;
  mVariableList.push_back( temp );
} // Parser::NewVariable()

int Parser::VariableExist( string name ) {
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( name == mVariableList[i].name ) return i;
  } // end for

  return -1;
} // Parser::VariableExist()

float Parser::StringToFloat( string num ) {
  float res = 0;
  bool mul = true;
  float base = 1;
  bool end = false;

  for ( int i = 0 ; ( ! end ) && i < num.length() ; i++ ) {
    if ( num[i] == '.' ) {
      mul = false;
      mIsInt = false;
      mCountAsFloat = true;
      base = 0.1;
    } // end if
    else {
      if ( mul ) {
        res = res * 10 + ( float ) num[i] - '0';
      } // end if
      else {
        res = res + ( ( float ) num[i] - '0' ) * base;
        if ( base != 0.0001 ) base = base / 10;
        else end = true;
      } // end else
    } // end else
  } // end for

  return res;
} // Parser::StringToFloat()

float Parser::GetVariableValue( string name ) {
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( name == mVariableList[i].name ) {
      if ( mIsInt ) mIsInt = mVariableList[i].isInt;
      if ( ! mCountAsFloat ) mCountAsFloat = mVariableList[i].isInt;
      return mVariableList[i].val;
    } // end if
  } // end for

  return 0.0;
} // Parser::GetVariableValue()

int main() {
  int uTestNum = 0 ;
  char ch;
  Parser parser; 
  bool get = true;
  if ( ! DEBUG ) scanf( "%d%c", &uTestNum, &ch );
  
  printf( "Program starts...\n" ) ;
  printf( "> " );
  
  parser.mScanner.Init();
  parser.Init();

  while ( parser.IsCommand() ) {
    ;
  } // while
  
  printf( "Program exits..." ) ;
} // main()
