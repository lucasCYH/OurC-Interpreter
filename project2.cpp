# include <stdlib.h>
# include <stdio.h>
# include <string>
# include <string.h>
# include <iostream>
# include <vector>

# define DEBUG 0

using namespace std;

enum TokenType {
  ID, CONSTANT, 
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
  string type;
  string val;
  int region;
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
  
    if ( mCur == '_' || ( mCur >= 'a' && mCur <= 'z' ) || ( mCur >= 'A' && mCur <= 'Z' ) ) {
      GetID();
      mToken.tokenType = ID;
    } // end if
    else if ( mCur >= '0' && mCur <= '9' ) {
      GetNumber();
      mToken.tokenType = CONSTANT;
    } // end if
    else if ( mCur == '\"' ) {
      GetDoubleQuote();
      mToken.tokenType = CONSTANT;
    } // end if
    else if ( mCur == '\'' ) {
      GetSingleQuote();
      mToken.tokenType = CONSTANT;
    } // end if
    else if ( mCur == '(' || mCur == ')' || mCur == '[' || mCur == ']' || mCur == '{' || mCur == '}' ) {
      GetParentheses();   
    } // end if
    else if ( mCur == '+' || mCur == '-' || mCur == '*' || mCur == '/' || mCur == '%' ) {
      GetArithmeticOp();
    } // end if
    else if ( mCur == '^' || mCur == '&' || mCur == '|' ) {
      GetBitOp();
    } // end if
    else if ( mCur == '>' || mCur == '<' || mCur == '=' || mCur == '!' ) {
      GetCompareOp();
    } // end if
    else if ( mCur == ';' || mCur == ',' || mCur == '?' || mCur == ':' ) {
      GetOtherSpecial();
    } // end if
    else {
      // printf( "char undefined! : %c", mCur );
      mToken.tokenString = mToken.tokenString + mCur;
      string errorMsg = "unrecognized token with first char : '" ;
      errorMsg = errorMsg + mCur + "'00";
      if ( DEBUG ) cout << errorMsg << endl;
      throw ( errorMsg );
    } // end else
  } // end while
  
  
  
  if ( mToken.tokenType == ID ) CheckReservedWords();

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
  int checkInput = GetNextChar();
      
  while ( ( ! end ) && ( ( mCur >= '0' && mCur <= '9' ) || mCur == '.' )  ) {
    if ( checkInput == 0 ) {
      end = true;
    } // end if

    if ( ! end ) {
      mToken.tokenString += mCur;
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
  else if ( mCur == '[' ) {
    mToken.tokenString += mCur;
    mToken.tokenType = MLP;
  } // end if
  else if ( mCur == ']' ) {
    mToken.tokenString += mCur;
    mToken.tokenType = MRP;
  } // end if
  else if ( mCur == '{' ) {
    mToken.tokenString += mCur;
    mToken.tokenType = BLP;
  } // end if
  else if ( mCur == '}' ) {
    mToken.tokenString += mCur;
    mToken.tokenType = BRP;
  } // end if
   
  GetNextChar();
  return;
} // Scanner::GetParentheses()
   
void Scanner::GetArithmeticOp() {
       
  int checkInput;
      
  if ( mCur == '+' ) {
    mToken.tokenString += mCur;
        
    checkInput = GetNextChar();
         
    if ( checkInput == 0 ) {
      mToken.tokenType = ADD;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = PE;
    } // end if
    else if ( mCur == '+' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = PP;
    } // end if
    else {
      mToken.tokenType = ADD;
    } // else
      
    if ( mToken.tokenType != ADD ) GetNextChar();
  } // end if
  else if ( mCur == '-' ) {
    mToken.tokenString += mCur;
         
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = SUB;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = ME;
    } // end if
    else if ( mCur == '-' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = MM;
    } // end if
    else {
      mToken.tokenType = SUB;
    } // else
      
    if ( mToken.tokenType != SUB ) GetNextChar();
  } // end if
  else if ( mCur == '*' ) {
    mToken.tokenString += mCur;
      
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = MULT;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = TE;
    } // end if
    else {
      mToken.tokenType = MULT;
    } // else
      
    if ( mToken.tokenType != MULT ) GetNextChar();
  } // end if
  else if ( mCur == '/' ) {
    mToken.tokenString += mCur;
     
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = DIV;
      return;
    } // end if
    else if ( mCur == '*' ) { // multi-line comment
      SkipMultiLineComment();
      mToken.tokenString = "";
      mGetToken = false;
      return;
    } // end if
    else if ( mCur == '/' ) { // single line comment
      SkipLineComment();
      mToken.tokenString = "";
      mGetToken = false;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = DE;
    } // end if
    else {
      mToken.tokenType = DIV;
    } // else
      
    if ( mToken.tokenType != DIV ) GetNextChar();
  } // end if
  else if ( mCur == '%' ) {
    mToken.tokenString += mCur;
      
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = REM;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = RE;
    } // end if
    else {
      mToken.tokenType = REM;
    } // else
      
    if ( mToken.tokenType != REM ) GetNextChar();
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
    else if ( mCur == '>' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = RS;
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
    else if ( mCur == '<' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = LS;
      GetNextChar();
    } // end if
    else {
      mToken.tokenType = LSS;
    } // else
  } // end if
  else if ( mCur == '=' ) {
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = ASSIGN;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = EQ;
      GetNextChar();
    } // end if
    else {
      mToken.tokenType = ASSIGN;
    } // else
  }  // end if
  else if ( mCur == '!' ) {
    checkInput = GetNextChar();
      
    if ( checkInput == 0 ) {
      mToken.tokenType = BITNOT;
      return;
    } // end if
    else if ( mCur == '=' ) {
      mToken.tokenString += mCur;
      mToken.tokenType = NEQ;
      GetNextChar();
    } // end if
    else {
      mToken.tokenType = BITNOT;
    } // else
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

/* 
void Scanner::PrintAllTokens() {
  string types[] =   {"ID", "CONSTANT", 
                  "INT", "FLOAT", "CHAR", "BOOL", "STRING", "VOID",
                  "IF", "ELSE", "WHILE", "DO", "RETURN", "CIN", "COUT",
                  "LP", "RP", "MLP", "MRP", "BLP", "BRP", // (), [], {}
                  "ADD", "SUB", "MULT", "DIV", "REM",  // +, -, *, /, %  
                  "BITAND", "BITOR", "BITXOR", "BITNOT",  // &, |, ^, !
                  "EQ", "NEQ", "LSS", "LE", "GTR", "GE", "LS", "RS",  // ==, !=, <, <=, >, >=, <<, >>
                  "AND", "OR", "PE", "ME", "TE", "DE", "RE", "PP", "MM", 
                  // &&, ||, +=, -=, *=, /=, %=, ++, --
                  "SCLN", "COMMA", "QUE", "COLON", "ASSIGN", // ';', ',', '?', ':', '='
                  "ADDORSIGN", "SUBORSIGN", // + & -
                  "INPUTBUFFERAPPEND", "ENDINPUTBUFFERAPPEND",
                  "NONE", "END_OF_FILE", "COMMENT"};  // ï¿½ï¿½lï¿½ï¿½
      
  for ( int i = 0 ; i < mTokenSet.size() ; i++ ) {
    cout << "Line " << mTokenSet[i].lineNum << ": " << mTokenSet[i].tokenString << 
    ", " << types[mTokenSet[i].tokenType] << endl;
  } // for()
} // Scanner::PrintAllTokens()

*/

void Scanner::CheckReservedWords() {
  if ( mToken.tokenString == "int" ) {
    mToken.tokenType = INT;
  } // end if
  else if ( mToken.tokenString == "float" ) {
    mToken.tokenType = FLOAT;
  } // end if
  else if ( mToken.tokenString == "char" ) {
    mToken.tokenType = CHAR;
  } // end if
  else if ( mToken.tokenString == "bool" ) {
    mToken.tokenType = BOOL;
  } // end if
  else if ( mToken.tokenString == "string" ) {
    mToken.tokenType = STRING;
  } // end if
  else if ( mToken.tokenString == "void" ) {
    mToken.tokenType = VOID;
  } // end if
  else if ( mToken.tokenString == "if" ) {
    mToken.tokenType = IF;
  } // end if
  else if ( mToken.tokenString == "else" ) {
    mToken.tokenType = ELSE;
  } // end if
  else if ( mToken.tokenString == "while" ) {
    mToken.tokenType = WHILE;
  } // end if
  else if ( mToken.tokenString == "do" ) {
    mToken.tokenType = DO;
  } // end if
  else if ( mToken.tokenString == "return" ) {
    mToken.tokenType = RETURN;
  } // end if
  else if ( mToken.tokenString == "cin" ) {
    mToken.tokenType = CIN;
  } // end if
  else if ( mToken.tokenString == "cout" ) {
    mToken.tokenType = COUT;
  } // end if
  else if ( mToken.tokenString == "InputBufferAppend" ) {
    mToken.tokenType = INPUTBUFFERAPPEND;
  } // end if
  else if ( mToken.tokenString == "EndInputBufferAppend" ) {
    mToken.tokenType = ENDINPUTBUFFERAPPEND;
  } // end if

  return;
} // Scanner::CheckReservedWords()

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

class Parser {
  public:
  bool IsUserInput() ;
  void Init() ;
  Scanner mScanner;
  private:
  int mIfStatement;
  int mRegion;
  int mThreshold;
  bool mLastStatementIf;
  bool mJustPeek;
  Token mCurToken;
  vector<Variable> mVariableList;
  vector<Function> mFunctionList;
  vector<Declarators> mDeclaratorsList;

  
  void ListFunction( string name ) ;
  void ListVariable( string name ) ;
  void ListAllVariables() ;
  void ListAllFunctions() ;
  
  bool IsDefOrStatement() ;
  void UserInput() ;
  void Definition( bool isBLP ) ;
  void TypeSpecifier() ;
  void FunctionDefinitionOrDeclarators( string type, string name, bool isBLP ) ;
  void RestOfDeclarators( string variableType, string variableName, bool isBLP ) ;
  void FunctionDefinitionWithoutID() ;
  void FormalParameterList() ;
  void CompoundStatement() ;
  void Declaration( bool isBLP ) ;
  void Statement() ;
  void StatementWhile() ;
  bool StatementIf() ;
  void Expression() ;
  void CoutExpression() ;
  void CinExpression() ;
  void BasicExpression() ;
  void RestOfIDStartedBasicExp( string name ) ;
  void RestOfPPMMIDStartedBasicExp() ;
  void Sign() ;
  void ActualParameterList() ;
  void AssignmentOperator() ;
  void RomceAndRomloe() ;
  void RestOfMaybeLogicalOrExp() ;
  void MaybeLogicalAndExp() ;
  void RestOfMaybeLogicalAndExp() ;
  void MaybeBitOrExp() ;
  void RestOfMaybeBitOrExp() ;
  void MaybeBitExOrExp() ;
  void RestOfMaybeBitExOrExp() ;
  void MaybeBitAndExp() ;
  void RestOfMaybeBitAndExp() ;
  void MaybeEqualityExp() ;
  void RestOfMaybeEqualityExp() ;
  void MaybeRelationalExp() ;
  void RestOfMaybeRelationalExp() ;
  void MaybeShiftExp() ;
  void RestOfMaybeShiftExp() ;
  void MaybeAdditiveExp() ;
  void RestOfMaybeAdditiveExp() ;
  void MaybeMultExp() ;
  void RestOfMaybeMultExp() ;
  void UnaryExp() ;
  void SignedUnaryExp() ;
  void UnsignedUnaryExp() ;
  
  
  void StatementOp() ;
  void DefinitionOp( bool isBLP ) ;
  int FindVariable( string name, bool printError ) ;
  int FindFunctionName( string name ) ;
  void CheckFunctionNameExist( string name ) ;
  void FunctionDefinitionWithoutIDOp() ;
  void RemoveVariables() ;
  void VariableSort() ;
  void FunctionSort() ;
  bool FindElse( int findIf ) ;
  bool JustIfStatement() ;
}; // Parser

void Parser::Init() {
  mVariableList.clear();
  mFunctionList.clear();
  mDeclaratorsList.clear();
  mScanner.mCurLine = 1;
  mCurToken = mScanner.GetToken();
} // Parser::Init()

bool Parser::IsUserInput() {
  mDeclaratorsList.clear();
  try {
    mLastStatementIf = false;
    mJustPeek = false;
    mRegion = 0;
    mIfStatement = 0;
    mThreshold = 0;
    UserInput();
  } catch ( string msg ) {
    if ( msg == "EOF" || msg == "Done" )
      return false;

    string num = msg.substr( msg.length() - 2, 2 );
    if ( num == "79" ) cout << "Line " << 
                            mScanner.mTokenList[mScanner.mTokenList.size() - 2].lineNum << " : ";
    else cout << "Line " << mCurToken.lineNum << " : ";

    if ( ! DEBUG ) msg = msg.substr( 0, msg.length() - 2 );
    cout << msg << endl;
    mRegion = 0;
    RemoveVariables();

    if ( num == "79" ) {
      if ( mScanner.mTokenList[mScanner.mTokenList.size() - 2].lineNum == mCurToken.lineNum ) {
        mScanner.mTokenList.clear();
        mScanner.SkipErrorLine();
        mScanner.LineInit();
        mCurToken = mScanner.GetToken();
      } // end if
      else { // hold the token
        mCurToken.lineNum = mCurToken.lineNum - 
                            mScanner.mTokenList[mScanner.mTokenList.size() - 2].lineNum;
        mScanner.mCurLine = mCurToken.lineNum;
        mScanner.mTokenList.clear();
        mScanner.mTokenList.push_back( mCurToken );
      } // end else

      return true;
    } // end if

    mScanner.mTokenList.clear();
    mScanner.SkipErrorLine();
    mScanner.LineInit();
    mCurToken = mScanner.GetToken();
    return true;
    
    return true;
  } // catch

  if ( mJustPeek ) { // hold the token
    mScanner.mTokenList.clear();
    mCurToken.tokenString = mScanner.mToken.tokenString;
    mCurToken.tokenType = mScanner.mToken.tokenType;
    mCurToken.lineNum = mScanner.mToken.lineNum - mCurToken.lineNum + 1;
    mScanner.mCurLine = mCurToken.lineNum;
    mScanner.mTokenList.push_back( mCurToken );
    mRegion = 0;
    RemoveVariables();
    return true;
  } // end if

  mScanner.LineInit();
  mCurToken = mScanner.GetToken();
  mScanner.mTokenList.clear();
  if ( mCurToken.lineNum == 0 ) {
    mCurToken.lineNum = 1;
    mScanner.mCurLine = 1;
  } // end if

  mScanner.mTokenList.push_back( mCurToken );
  mRegion = 0;
  RemoveVariables();
    

  if ( DEBUG ) cout << "The grammer is correct" << endl;
  return true;
} // Parser::IsUserInput()

void Parser::UserInput() {
  // Token curToken = mScanner.GetMemberToken();
  if ( DEBUG ) cout << "UserInput" << endl;

  
  
  if ( mCurToken.tokenType == END_OF_FILE ) {
    throw ( "EOF" );
  } // end if
  else if ( mCurToken.tokenType == VOID || 
            mCurToken.tokenType == INT || 
            mCurToken.tokenType == CHAR || 
            mCurToken.tokenType == FLOAT || 
            mCurToken.tokenType == STRING || 
            mCurToken.tokenType == BOOL ) { // the first type of definition
    Definition( false );
  } // end if
  else if ( mCurToken.tokenType == ID ||
            mCurToken.tokenType == PP ||
            mCurToken.tokenType == MM ||
            mCurToken.tokenType == ADD ||
            mCurToken.tokenType == SUB ||
            mCurToken.tokenType == BITNOT ||
            mCurToken.tokenType == CONSTANT ||
            mCurToken.tokenType == RETURN ||
            mCurToken.tokenType == BLP ||
            mCurToken.tokenType == IF || 
            mCurToken.tokenType == WHILE ||
            mCurToken.tokenType == DO ||
            mCurToken.tokenType == SCLN ||
            mCurToken.tokenType == LP ||
            mCurToken.tokenType == CIN ||
            mCurToken.tokenType == COUT ) {
    Statement();
    StatementOp();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'01";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of UserInput" << endl; 
} // Parser::UserInput()

void Parser::Definition( bool isBLP ) {
  if ( DEBUG ) cout << "Definition" << endl;
  if ( mCurToken.tokenType == VOID ) { // should be a function
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mCurToken.tokenType == ID ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == LP ) FunctionDefinitionWithoutID(); 
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'02";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'02";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mCurToken.tokenType == INT || 
            mCurToken.tokenType == CHAR || 
            mCurToken.tokenType == FLOAT || 
            mCurToken.tokenType == STRING || 
            mCurToken.tokenType == BOOL ) {
              
    string type = mCurToken.tokenString;
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mCurToken.tokenType == ID ) {
      FunctionDefinitionOrDeclarators( type, mCurToken.tokenString, isBLP ); 
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'03";
      throw ( errorMsg );
    } // end else
    
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'04";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of Definition" << endl;
} // Parser::Definition()

void Parser::TypeSpecifier() {
  if ( DEBUG ) cout << "TypeSpecifier" << endl;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE )  {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mCurToken.tokenType != INT && mCurToken.tokenType != CHAR && mCurToken.tokenType != FLOAT &&
            mCurToken.tokenType != STRING && mCurToken.tokenType != BOOL ) {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'05";
    throw ( errorMsg );
  } // end if
  
  if ( DEBUG ) cout << "end of TypeSpecifier" << endl;
} // Parser::TypeSpecifier()

void Parser::FunctionDefinitionOrDeclarators( string type, string name, bool isBLP ) {
  if ( DEBUG ) cout << "FunctionDefinitionOrDeclarators" << endl;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mCurToken.tokenType == LP ) {
    
    // record the type and name of the function
    
    FunctionDefinitionWithoutID();
  } // end if
  else if ( mCurToken.tokenType == MLP || mCurToken.tokenType == SCLN || mCurToken.tokenType == COMMA ) {
    RestOfDeclarators( type, name, isBLP );
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'06";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of FunctionDefinitionOrDeclarators" << endl;
} // Parser::FunctionDefinitionOrDeclarators()

void Parser::RestOfDeclarators( string variableType, string variableName, bool isBLP ) {
  Declarators temp;
  temp.type = variableType;
  temp.name = variableName;
  temp.num = 1;
  mDeclaratorsList.push_back( temp );
  if ( DEBUG ) cout << "RestOfDeclarators" << endl;
  if ( mCurToken.tokenType == MLP ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mCurToken.tokenType == CONSTANT ) {
      string length = mCurToken.tokenString;
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mCurToken.tokenType != MRP ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'07";
        throw ( errorMsg );
      } // end if
      else if ( mCurToken.tokenType == MRP ) {
        mCurToken = mScanner.GetToken();
        mDeclaratorsList[0].num = StringToInt( length );
      } // end if
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'08";
      throw ( errorMsg );
    } // end else
  } // end if

  if ( mCurToken.tokenType == COMMA ) {
    while ( mCurToken.tokenType == COMMA ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mCurToken.tokenType == ID ) { // check is there any '[' CONSTANT ']' pattern next
        temp.name = mCurToken.tokenString;
        mCurToken = mScanner.GetToken();
        if ( mCurToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if
        else if ( mCurToken.tokenType == MLP ) { // check '['
          mCurToken = mScanner.GetToken();
          if ( mCurToken.tokenType == END_OF_FILE ) {
            string error = "EOF";
            throw ( error );
          } // end if
          else if ( mCurToken.tokenType == CONSTANT ) {  // check CONSTANT
            string length = mCurToken.tokenString;
            mCurToken = mScanner.GetToken();
            if ( mCurToken.tokenType == END_OF_FILE ) {
              string error = "EOF";
              throw ( error );
            } // end if
            else if ( mCurToken.tokenType != MRP ) {  // check ']'
              string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'09";
              throw ( errorMsg );
            } // end if
            else { // the pattern is correct
              temp.num = StringToInt( length );
              temp.type = variableType;
              mDeclaratorsList.push_back( temp );
              mCurToken = mScanner.GetToken();
            } // end else
          } // end if
          else {
            string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'10";
            throw ( errorMsg );
          } // end else
        } // end if
        else {
          temp.num = 1;
          temp.type = variableType;
          mDeclaratorsList.push_back( temp );
        } // end else
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'11";
        throw ( errorMsg );
      } // end else
    } // end while
  } // end if
  
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mCurToken.tokenType != SCLN ) {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'12";
    throw ( errorMsg );
  } // end if
  
  DefinitionOp( isBLP );

  if ( DEBUG ) cout << "end of RestOfDeclarators" << endl;
} // Parser::RestOfDeclarators()

void Parser::FunctionDefinitionWithoutID() {
  if ( DEBUG ) cout << "FunctionDefinitionWithoutID" << endl;
  mRegion++;
  mThreshold = mRegion;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mCurToken.tokenType == INT || 
            mCurToken.tokenType == CHAR || 
            mCurToken.tokenType == FLOAT || 
            mCurToken.tokenType == STRING || 
            mCurToken.tokenType == BOOL ) {
    FormalParameterList();
  } // end if

  if ( mCurToken.tokenType == RP ) mCurToken = mScanner.GetToken();
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'14";
    throw ( errorMsg );
  } // end else

  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mCurToken.tokenType == BLP ) {
    CompoundStatement();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'14";
    throw ( errorMsg );
  } // end else
  
  FunctionDefinitionWithoutIDOp();
  mRegion = 0;
  mThreshold = mRegion;
  RemoveVariables();
  
  
  if ( DEBUG ) cout << "end of FunctionDefinitionWithoutID" << endl;
} // Parser::FunctionDefinitionWithoutID()

void Parser::FormalParameterList() {
  if ( DEBUG ) cout << "FormalParameterList" << endl;

  Declarators temp;
  temp.type = mCurToken.tokenString;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  
  if ( mCurToken.tokenType == BITAND ) { // check for '&'
    mCurToken = mScanner.GetToken();
  } // end if
  
  if ( mCurToken.tokenType != ID ) {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'15";
    throw ( errorMsg );
  } // end if
  else {
    temp.name = mCurToken.tokenString;
  } // end else

  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == MLP ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mCurToken.tokenType == CONSTANT ) {
      temp.num = StringToInt( mCurToken.tokenString );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mCurToken.tokenType != MRP ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'16";
        throw ( errorMsg );
      } // end if
      else mCurToken = mScanner.GetToken();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'17";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    temp.num = 1;
  } // end else

  mDeclaratorsList.push_back( temp );

  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  while ( mCurToken.tokenType == COMMA ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mCurToken.tokenType != INT && 
         mCurToken.tokenType != CHAR && 
         mCurToken.tokenType != FLOAT && 
         mCurToken.tokenType != STRING && 
         mCurToken.tokenType != BOOL ) {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'18";
      throw ( errorMsg );
    } // end if
    else {
      temp.type = mCurToken.tokenString;
    } // end else
           
    mCurToken = mScanner.GetToken();
  
    if ( mCurToken.tokenType == BITAND ) { // check for '&' (optional)
      mCurToken = mScanner.GetToken();
    } // end if
      
      
      
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mCurToken.tokenType == ID ) { // check is there any '[' CONSTANT ']' pattern next
      temp.name = mCurToken.tokenString;
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mCurToken.tokenType == MLP ) { // check '['
        mCurToken = mScanner.GetToken();
        if ( mCurToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if
        else if ( mCurToken.tokenType == CONSTANT ) {  // check CONSTANT
          temp.num = StringToInt( mCurToken.tokenString );
          mCurToken = mScanner.GetToken();
          if ( mCurToken.tokenType == END_OF_FILE ) {
            string error = "EOF";
            throw ( error );
          } // end if
          else if ( mCurToken.tokenType != MRP ) {  // check ']'
            string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'19";
            throw ( errorMsg );
          } // end if
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'20";
          throw ( errorMsg );
        } // end else
      } // end if
      else {
        temp.num = 1;
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'21";
      throw ( errorMsg );
    } // end else

    mDeclaratorsList.push_back( temp );
  } // end while
  
  DefinitionOp( true );
  if ( DEBUG ) cout << "end of FormalParameterList" << endl;
} // Parser::FormalParameterList()

void Parser::CompoundStatement() {
  mRegion++;
  int findIf = -1;

  if ( DEBUG ) cout << "CompoundStatement" << endl;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( IsDefOrStatement() ) {
    while ( IsDefOrStatement() ) {
           
      if ( mCurToken.tokenType == INT || 
           mCurToken.tokenType == CHAR || 
           mCurToken.tokenType == FLOAT || 
           mCurToken.tokenType == STRING || 
           mCurToken.tokenType == BOOL ) {
        Declaration( true );
      } // end if
      else if ( mCurToken.tokenType == ID ||
                mCurToken.tokenType == PP ||
                mCurToken.tokenType == MM ||
                mCurToken.tokenType == ADD ||
                mCurToken.tokenType == SUB ||
                mCurToken.tokenType == BITNOT ||
                mCurToken.tokenType == CONSTANT ||
                mCurToken.tokenType == RETURN ||
                mCurToken.tokenType == BLP ||
                mCurToken.tokenType == IF || 
                mCurToken.tokenType == WHILE ||
                mCurToken.tokenType == DO ||
                mCurToken.tokenType == SCLN ||
                mCurToken.tokenType == LP ||
                mCurToken.tokenType == CIN ||
                mCurToken.tokenType == COUT ) {

        Statement();
      } // end if
      
      if ( ! mJustPeek ) mCurToken = mScanner.GetToken();
      else {
        mCurToken = mScanner.mToken;
        mJustPeek = false;
      } // end else

      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
    } // end while
  } // end if
  
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mCurToken.tokenType != BRP ) {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'23";
    throw ( errorMsg );
  } // end if
  else {
    mRegion--;
    RemoveVariables();
  } // end else
  
  if ( DEBUG ) cout << "end of CompoundStatement" << endl;
  mLastStatementIf = false;
} // Parser::CompoundStatement()

void Parser::Declaration( bool isBLP ) {
  if ( DEBUG ) cout << "Declaration" << endl;
  string type = mCurToken.tokenString;
  string name;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( mCurToken.tokenType == ID ) {
    name = mCurToken.tokenString;
    mCurToken = mScanner.GetToken();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'24";
    throw ( errorMsg );
  } // end else
  
  if ( mCurToken.tokenType == MLP || mCurToken.tokenType == SCLN || mCurToken.tokenType == COMMA ) {
    RestOfDeclarators( type, name, isBLP );
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'25";
    throw ( errorMsg );
  } // end else
  
  if ( DEBUG ) cout << "end of Declaration" << endl;
} // Parser::Declaration()

void Parser::Statement() {
  if ( DEBUG ) cout << "Statement" << endl;
  mLastStatementIf = false;
  
  if ( DEBUG ) cout << mCurToken.tokenString << mIfStatement << endl;
  
  if ( mCurToken.tokenType == SCLN ) return; // condition 1
  else if ( mCurToken.tokenType == ID ||
            mCurToken.tokenType == PP ||
            mCurToken.tokenType == MM ||
            mCurToken.tokenType == ADD ||
            mCurToken.tokenType == SUB ||
            mCurToken.tokenType == BITNOT ||
            mCurToken.tokenType == CONSTANT ||
            mCurToken.tokenType == LP ) { // condition 2

    Expression();
    
    if ( mCurToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'26";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mCurToken.tokenType == RETURN ) { // condition 3
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mCurToken.tokenType == ID ||
         mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM ||
         mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB ||
         mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT ||
         mCurToken.tokenType == LP ) {
      
      Expression();
    } // end if
    
    if ( mCurToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'27";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mCurToken.tokenType == BLP ) { // condition 4
    CompoundStatement();
  } // end if
  else if ( mCurToken.tokenType == IF ) { // condition 5
    mLastStatementIf = StatementIf();
  } // end if
  else if ( mCurToken.tokenType == WHILE ) { // condition 6
    StatementWhile();
  } // end if
  else if ( mCurToken.tokenType == DO ) { // condition 7
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    Statement();

    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mCurToken.tokenType == WHILE ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mCurToken.tokenType == LP ) {
        mCurToken = mScanner.GetToken();
        if ( mCurToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if

        if ( mCurToken.tokenType == ID ||
             mCurToken.tokenType == PP ||
             mCurToken.tokenType == MM ||
             mCurToken.tokenType == ADD ||
             mCurToken.tokenType == SUB ||
             mCurToken.tokenType == BITNOT ||
             mCurToken.tokenType == CONSTANT ||
             mCurToken.tokenType == ID ||
             mCurToken.tokenType == LP ) {
          Expression();
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'34";
          throw ( errorMsg );
        } // end else
    
        if ( mCurToken.tokenType != RP ) {
          string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'35";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'29";
        throw ( errorMsg );
      } // end else
      
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mCurToken.tokenType != SCLN ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'30";
        throw ( errorMsg );
      } // end if
    } // end if
  } // end if
  else if ( mCurToken.tokenType == ELSE && mIfStatement ) {
    mIfStatement--;
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    Statement();
  } // end if
  else if ( mCurToken.tokenType == COUT ) {
    CoutExpression();
  } // end if
  else if ( mCurToken.tokenType == CIN ) {
    CinExpression();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'3'";
    throw ( errorMsg );
  } // end else
  
  if ( DEBUG ) cout << "end of Statement" << endl;
} // Parser::Statement()

void Parser::CoutExpression() {
  if ( DEBUG ) cout << "CoutExpression" << endl;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if

  if ( mCurToken.tokenType == LS ) {
    while ( mCurToken.tokenType == LS ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mCurToken.tokenType == ID || mCurToken.tokenType == LP || mCurToken.tokenType == CONSTANT ) {
        UnsignedUnaryExp() ;
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'33";
        throw ( errorMsg );
      } // end else
    } // end while

    if ( mCurToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'32";
      throw ( errorMsg );
    } // end if
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'33";
    throw ( errorMsg );
  } // end else
} // Parser::CoutExpression()

void Parser::CinExpression() {
  if ( DEBUG ) cout << "CinExpression" << endl;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if

  if ( mCurToken.tokenType == RS ) {
    while ( mCurToken.tokenType == RS ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mCurToken.tokenType == ID || mCurToken.tokenType == LP || mCurToken.tokenType == CONSTANT ) {
        UnsignedUnaryExp() ;
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'33";
        throw ( errorMsg );
      } // end else
    } // end while

    if ( mCurToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'35";
      throw ( errorMsg );
    } // end if
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'36";
    throw ( errorMsg );
  } // end else
} // Parser::CinExpression()

bool Parser::StatementIf() {
  if ( DEBUG ) cout << " StatementIf" << endl;
  mCurToken = mScanner.GetToken();
  mIfStatement++;

  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
    
  if ( mCurToken.tokenType == LP ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mCurToken.tokenType == ID ||
         mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM ||
         mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB ||
         mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT || 
         mCurToken.tokenType == LP ) {
      Expression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'31";
      throw ( errorMsg );
    } // end else
    
    if ( mCurToken.tokenType != RP ) {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'32";
      throw ( errorMsg );
    } // end if
    
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mCurToken.tokenType == ID ||
         mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM ||
         mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB ||
         mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT ||
         mCurToken.tokenType == RETURN ||
         mCurToken.tokenType == BLP ||
         mCurToken.tokenType == IF || 
         mCurToken.tokenType == WHILE ||
         mCurToken.tokenType == DO ||
         mCurToken.tokenType == SCLN ||
         mCurToken.tokenType == CIN ||
         mCurToken.tokenType == COUT ||
         mCurToken.tokenType == LP ) {
      Statement();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'33";
      throw ( errorMsg );
    } // end else
    
      
    mScanner.GetToken();
    
    if ( mScanner.mToken.tokenType == ELSE ) {
      mCurToken = mScanner.mToken;
      Statement();
      return false;
    } // end if
    else {
      mJustPeek = true;
    } // end else
  } // end if

  if ( DEBUG ) cout << " end of StatementIf" << endl;

  return true;
} // Parser::StatementIf()

void Parser::StatementWhile() {
  if ( DEBUG ) cout << "StatementWhile" << endl;
  mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
    
  if ( mCurToken.tokenType == LP ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mCurToken.tokenType == ID ||
         mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM ||
         mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB ||
         mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT ||
         mCurToken.tokenType == ID ||
         mCurToken.tokenType == LP ) {
      Expression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'34";
      throw ( errorMsg );
    } // end else
    
    if ( mCurToken.tokenType != RP ) {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'35";
      throw ( errorMsg );
    } // end if
    
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mCurToken.tokenType == ID ||
         mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM ||
         mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB ||
         mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT ||
         mCurToken.tokenType == RETURN ||
         mCurToken.tokenType == BLP ||
         mCurToken.tokenType == IF || 
         mCurToken.tokenType == WHILE ||
         mCurToken.tokenType == DO ||
         mCurToken.tokenType == SCLN ||
         mCurToken.tokenType == CIN ||
         mCurToken.tokenType == COUT ||
         mCurToken.tokenType == LP ) {
      Statement();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'36";
      throw ( errorMsg );
    } // end else
  } // end if
  
  if ( DEBUG ) cout << "end of StatementWhile" << endl;
} // Parser::StatementWhile()

bool Parser::IsDefOrStatement() {
  if ( DEBUG ) cout << "IsDefOrStatement" << endl;
  return ( mCurToken.tokenType == INT || 
           mCurToken.tokenType == CHAR || 
           mCurToken.tokenType == FLOAT || 
           mCurToken.tokenType == STRING || 
           mCurToken.tokenType == BOOL ||
           mCurToken.tokenType == ID ||
           mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM ||
           mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB ||
           mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT ||
           mCurToken.tokenType == RETURN ||
           mCurToken.tokenType == BLP ||
           mCurToken.tokenType == IF || 
           mCurToken.tokenType == WHILE ||
           mCurToken.tokenType == DO ||
           mCurToken.tokenType == SCLN ||
           mCurToken.tokenType == LP ||
           mCurToken.tokenType == CIN ||
           mCurToken.tokenType == COUT );
} // Parser::IsDefOrStatement()

void Parser::Expression() {
  if ( DEBUG ) cout << "Expression" << endl;
  BasicExpression();
  
  if ( mCurToken.tokenType == COMMA ) {
    while ( mCurToken.tokenType == COMMA ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mCurToken.tokenType == ID ||
           mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM ||
           mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB ||
           mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT ||
           mCurToken.tokenType == LP ) {
        BasicExpression();
      } // end if
      
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    } // end while
  } // end if
  
  if ( DEBUG ) cout << "end of Expression" << endl;
} // Parser::Expression()

void Parser::BasicExpression() {
  if ( DEBUG ) cout << "BasicExpression" << endl;
  if ( mCurToken.tokenType == ID ) {
    // FindVariable( mCurToken.tokenString, true );
    string name = mCurToken.tokenString;
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) { 
      string error = "EOF";
      throw ( error );
    } // end if
    
    RestOfIDStartedBasicExp( name );
  } // end if
  else if ( mCurToken.tokenType == PP || mCurToken.tokenType == MM ) { // condition 2
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mCurToken.tokenType == ID ) {
      FindVariable( mCurToken.tokenString, true );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      RestOfPPMMIDStartedBasicExp();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'38";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mCurToken.tokenType == ADD || mCurToken.tokenType == SUB || 
            mCurToken.tokenType == BITNOT ) { // condition 3
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    while ( mCurToken.tokenType == ADD || mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    } // end while
    
    if ( mCurToken.tokenType == ID || mCurToken.tokenType == CONSTANT || 
         mCurToken.tokenType == LP ) {
      SignedUnaryExp();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'39";
      throw ( errorMsg );
    } // end else
  
    RomceAndRomloe();
  } // end if
  else if ( mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) { // condition 4
    if ( mCurToken.tokenType == LP ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    
      if ( mCurToken.tokenType == ID ||
           mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM ||
           mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB ||
           mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT ||
           mCurToken.tokenType == ID ||
           mCurToken.tokenType == LP ) {
        Expression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'40";
        throw ( errorMsg );
      } // end else
      
      if ( mCurToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'41";
        throw ( errorMsg );      
      } // end if
    } // end if
  
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if	

    RomceAndRomloe();
  } // end if
  
  if ( DEBUG ) cout << "end of BasicExpression" << endl;
} // Parser::BasicExpression()

void Parser::RestOfIDStartedBasicExp( string name ) {
  if ( DEBUG ) cout << "RestOfIDStartedBasicExp" << endl;
  if ( mCurToken.tokenType == MLP || mCurToken.tokenType == ASSIGN || 
       mCurToken.tokenType == TE || mCurToken.tokenType == DE || 
       mCurToken.tokenType == RE || mCurToken.tokenType == PE || 
       mCurToken.tokenType == ME || mCurToken.tokenType == PP ||
       mCurToken.tokenType == MM || mCurToken.tokenType == ADD || 
       mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
       mCurToken.tokenType == ID || mCurToken.tokenType == CONSTANT ) { // condition 1

    if ( mCurToken.tokenType == MLP ) {
      FindVariable( name, true );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    
      if ( mCurToken.tokenType == ID ||
           mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM ||
           mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB ||
           mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT ||
           mCurToken.tokenType == ID ||
           mCurToken.tokenType == LP ) {
        Expression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'42";
        throw ( errorMsg );
      } // end else
    
      if ( mCurToken.tokenType != MRP ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'43";
        throw ( errorMsg );
      } // end if
      else {
        mCurToken = mScanner.GetToken();
      } // end else
    } // end if
    
    if ( mCurToken.tokenType == ASSIGN || mCurToken.tokenType == TE || 
         mCurToken.tokenType == DE || mCurToken.tokenType == RE || 
         mCurToken.tokenType == PE || mCurToken.tokenType == ME ) {

      FindVariable( name, true );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
        BasicExpression();
      } // end if
    } // end if
    else  {
      if ( mCurToken.tokenType == MM || mCurToken.tokenType == PP ) mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      FindVariable( name, true );
      RomceAndRomloe();
    } // end else
  } // end if
  else if ( mCurToken.tokenType == LP ) {
    CheckFunctionNameExist( name );
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
      ActualParameterList();
      
    } // end if

    if ( mCurToken.tokenType == RP ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      RomceAndRomloe();
      /*
      else  {
        string errorMsg = "unexpected token '" + mCurToken.tokenString + "'\n45";
        throw ( errorMsg );
      } // end else
      */
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'46";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    FindVariable( name, true );
    RomceAndRomloe();
  } // end else
  
  if ( DEBUG ) cout << "end of RestOfIDStartedBasicExp" << endl;
} // Parser::RestOfIDStartedBasicExp()

void Parser::RestOfPPMMIDStartedBasicExp() {
  if ( DEBUG ) cout << "RestOfPPMMIDStartedBasicExp" << endl;
  if ( mCurToken.tokenType == MLP ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mCurToken.tokenType == ID ||
         mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM ||
         mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB ||
         mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT ||
         mCurToken.tokenType == ID ||
         mCurToken.tokenType == LP ) {
      Expression();
      
      
      if ( mCurToken.tokenType == MRP ) {
        mCurToken = mScanner.GetToken();
        if ( mCurToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if
        
        if ( mCurToken.tokenType == PP || mCurToken.tokenType == LP ||
             mCurToken.tokenType == MM || mCurToken.tokenType == ADD || 
             mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
             mCurToken.tokenType == ID || mCurToken.tokenType == CONSTANT ) {

          RomceAndRomloe();
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'47";
          throw ( errorMsg );
        } // end else
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'48";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'49";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    RomceAndRomloe();
  } // end else
  
  if ( DEBUG ) cout << "end of RestOfPPMMIDStartedBasicExp" << endl;
} // Parser::RestOfPPMMIDStartedBasicExp() ;

void Parser::Sign() {
  if ( DEBUG ) cout << "Sign" << endl;
  if ( mCurToken.tokenType != ADD && mCurToken.tokenType != SUB && mCurToken.tokenType != BITNOT ) {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'50";
    throw ( errorMsg );
  } // end if
} // Parser::Sign()

void Parser::ActualParameterList() {
  if ( DEBUG ) cout << "ActualParameterList" << endl;
  BasicExpression();
  
  while ( mCurToken.tokenType == COMMA ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
      BasicExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'51";
      throw ( errorMsg );
    } // end else
    

  } // end while
  
  if ( DEBUG ) cout << "end of ActualParameterList" << endl;
} // Parser::ActualParameterList()

void Parser::AssignmentOperator() {
  if ( DEBUG ) cout << "AssignmentOperator" << endl;
  if ( mCurToken.tokenType != ASSIGN && mCurToken.tokenType != TE && 
       mCurToken.tokenType != DE && mCurToken.tokenType != RE &&
       mCurToken.tokenType != PE && mCurToken.tokenType != ME ) {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'52";
    throw ( errorMsg );
  } // end if
} // Parser::AssignmentOperator()

void Parser::RomceAndRomloe() {
  if ( DEBUG ) cout << "RomceAndRomloe" << endl;
  
  RestOfMaybeLogicalOrExp();
  
  // mCurToken = mScanner.GetToken();
  if ( mCurToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( mCurToken.tokenType == QUE ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
  
    if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
      BasicExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'53";
      throw ( errorMsg );
    } // end else
    
    if ( mCurToken.tokenType == COLON ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
        BasicExpression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'54";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'55";
      throw ( errorMsg );
    } // end else
  } // end if
  
  if ( DEBUG ) cout << "end of RomceAndRomloe" << endl;
} // Parser::RomceAndRomloe()

void Parser::RestOfMaybeLogicalOrExp() {
  if ( DEBUG ) cout << "RestOfMaybeLogicalOrExp" << endl;
  RestOfMaybeLogicalAndExp();
  
  
  while ( mCurToken.tokenType == OR ) {
    mCurToken = mScanner.GetToken();
    
    MaybeLogicalAndExp();
  } // end while

  if ( DEBUG ) cout << "end of RestOfMaybeLogicalOrExp" << endl;
} // Parser::RestOfMaybeLogicalOrExp()

void Parser::MaybeLogicalAndExp() {
  if ( DEBUG ) cout << "MaybeLogicalAndExp" << endl;
  MaybeBitOrExp();
  
  
  while ( mCurToken.tokenType == AND ) {
    mCurToken = mScanner.GetToken();
    
    MaybeBitOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeLogicalAndExp" << endl;
} // Parser::MaybeLogicalAndExp()

void Parser::RestOfMaybeLogicalAndExp() {
  if ( DEBUG ) cout << "RestOfMaybeLogicalAndExp" << endl;
  RestOfMaybeBitOrExp();
  
  while ( mCurToken.tokenType == AND ) {
    mCurToken = mScanner.GetToken();
    
    MaybeBitOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeLogicalAndExp" << endl;
} // Parser::RestOfMaybeLogicalAndExp()

void Parser::MaybeBitOrExp() {
  if ( DEBUG ) cout << "MaybeBitOrExp" << endl;
  MaybeBitExOrExp();
  
  while ( mCurToken.tokenType == BITOR ) {
    mCurToken = mScanner.GetToken();
    
    MaybeBitExOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitOrExp" << endl;
} // Parser::MaybeBitOrExp()

void Parser::RestOfMaybeBitOrExp() {
  if ( DEBUG ) cout << "RestOfMaybeBitOrExp" << endl;
  RestOfMaybeBitExOrExp();

  while ( mCurToken.tokenType == BITOR ) {
    mCurToken = mScanner.GetToken();
    
    MaybeBitExOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitOrExp" << endl;
} // Parser::RestOfMaybeBitOrExp()

void Parser::MaybeBitExOrExp() {
  if ( DEBUG ) cout << "MaybeBitExOrExp" << endl;
  MaybeBitAndExp();

  while ( mCurToken.tokenType == BITXOR ) {
    mCurToken = mScanner.GetToken();
    
    MaybeBitAndExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitExOrExp" << endl;
} // Parser::MaybeBitExOrExp()

void Parser::RestOfMaybeBitExOrExp() {
  if ( DEBUG ) cout << "RestOfMaybeBitExOrExp" << endl;
  RestOfMaybeBitAndExp();

  while ( mCurToken.tokenType == BITXOR ) {
    mCurToken = mScanner.GetToken();
    
    MaybeBitAndExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitExOrExp" << endl;
} // Parser::RestOfMaybeBitExOrExp()

void Parser::MaybeBitAndExp() {
  if ( DEBUG ) cout << "MaybeBitAndExp" << endl;
  MaybeEqualityExp();
  
  while ( mCurToken.tokenType == BITAND ) {
    mCurToken = mScanner.GetToken();
    
    MaybeEqualityExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitAndExp" << endl;
} // Parser::MaybeBitAndExp()

void Parser::RestOfMaybeBitAndExp() {
  if ( DEBUG ) cout << "RestOfMaybeBitAndExp" << endl;
  RestOfMaybeEqualityExp();

  while ( mCurToken.tokenType == BITAND ) {
    mCurToken = mScanner.GetToken();
    
    MaybeEqualityExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitAndExp" << endl;
} // Parser::RestOfMaybeBitAndExp()

void Parser::MaybeEqualityExp() {
  if ( DEBUG ) cout << "MaybeEqualityExp" << endl;
  MaybeRelationalExp();

  while ( mCurToken.tokenType == NEQ || mCurToken.tokenType == EQ ) {
    mCurToken = mScanner.GetToken();
    
    MaybeRelationalExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeEqualityExp" << endl;
} // Parser::MaybeEqualityExp()

void Parser::RestOfMaybeEqualityExp() {
  if ( DEBUG ) cout << "RestOfMaybeEqualityExp" << endl;
  RestOfMaybeRelationalExp();

  while ( mCurToken.tokenType == NEQ || mCurToken.tokenType == EQ ) {
    mCurToken = mScanner.GetToken();
    
    MaybeRelationalExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeEqualityExp" << endl;
} // Parser::RestOfMaybeEqualityExp()

void Parser::MaybeRelationalExp() {
  if ( DEBUG ) cout << "MaybeRelationalExp" << endl;
  MaybeShiftExp();

  while ( mCurToken.tokenType == GE || mCurToken.tokenType == LE ||
          mCurToken.tokenType == LSS || mCurToken.tokenType == GTR ) {
    mCurToken = mScanner.GetToken();
    
    MaybeShiftExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeRelationalExp" << endl;
} // Parser::MaybeRelationalExp()

void Parser::RestOfMaybeRelationalExp() {
  if ( DEBUG ) cout << "RestOfMaybeRelationalExp" << endl;
  RestOfMaybeShiftExp();

  while ( mCurToken.tokenType == GE || mCurToken.tokenType == LE ||
          mCurToken.tokenType == LSS || mCurToken.tokenType == GTR ) {
    mCurToken = mScanner.GetToken();
    
    MaybeShiftExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeRelationalExp" << endl;
} // Parser::RestOfMaybeRelationalExp()

void Parser::MaybeShiftExp() {
  if ( DEBUG ) cout << "MaybeShiftExp" << endl;
  MaybeAdditiveExp();
  
  while ( mCurToken.tokenType == RS || mCurToken.tokenType == LS ||
          mCurToken.tokenType == LE || mCurToken.tokenType == GE ) {
    mCurToken = mScanner.GetToken();
    
    MaybeAdditiveExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeShiftExp" << endl;
} // Parser::MaybeShiftExp()

void Parser::RestOfMaybeShiftExp() {
  if ( DEBUG ) cout << "RestOfMaybeShiftExp" << endl;
  RestOfMaybeAdditiveExp();
  
  while ( mCurToken.tokenType == RS || mCurToken.tokenType == LS ||
          mCurToken.tokenType == LE || mCurToken.tokenType == GE ) {
    mCurToken = mScanner.GetToken();
    
    MaybeAdditiveExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeShiftExp" << endl;
} // Parser::RestOfMaybeShiftExp()

void Parser::MaybeAdditiveExp() {
  if ( DEBUG ) cout << "MaybeAdditiveExp" << endl;
  MaybeMultExp();
  
  while ( mCurToken.tokenType == ADD || mCurToken.tokenType == SUB ) {
    mCurToken = mScanner.GetToken();
    
    MaybeMultExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeAdditiveExp" << endl;
} // Parser::MaybeAdditiveExp()

void Parser::RestOfMaybeAdditiveExp() {
  if ( DEBUG ) cout << "RestOfMaybeAdditiveExp" << endl;

  RestOfMaybeMultExp();
  
  while ( mCurToken.tokenType == ADD || mCurToken.tokenType == SUB ) {
    mCurToken = mScanner.GetToken();
    
    MaybeMultExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeAdditiveExp" << endl;
} // Parser::RestOfMaybeAdditiveExp()

void Parser::MaybeMultExp() {
  if ( DEBUG ) cout << "MaybeMultExp" << endl;

  UnaryExp();

  RestOfMaybeMultExp();
  
  if ( DEBUG ) cout << "end of MaybeMultExp" << endl;
} // Parser::MaybeMultExp()

void Parser::RestOfMaybeMultExp() {
  if ( DEBUG ) cout << "RestOfMaybeMultExp" << endl;

  while ( mCurToken.tokenType == MULT || mCurToken.tokenType == DIV || mCurToken.tokenType == REM ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    UnaryExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeMultExp" << endl;
} // Parser::RestOfMaybeMultExp()

void Parser::UnaryExp() {
  if ( DEBUG ) cout << mCurToken.tokenString << " UnaryExp" << endl;

  if ( mCurToken.tokenType == ADD || mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ) {
    mCurToken = mScanner.GetToken();
  
    while ( mCurToken.tokenType == ADD || mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ) {
      mCurToken = mScanner.GetToken();
    } // end while

    SignedUnaryExp();
  } // end if
  else if ( mCurToken.tokenType == ID || mCurToken.tokenType == LP || mCurToken.tokenType == CONSTANT ) {
    UnsignedUnaryExp();
  } // end if
  else if ( mCurToken.tokenType == PP || mCurToken.tokenType == MM ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == ID ) {
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == MLP ) {
        mCurToken = mScanner.GetToken();
        if ( mCurToken.tokenType == ID ||
             mCurToken.tokenType == PP ||
             mCurToken.tokenType == MM ||
             mCurToken.tokenType == ADD ||
             mCurToken.tokenType == SUB ||
             mCurToken.tokenType == BITNOT ||
             mCurToken.tokenType == CONSTANT ||
             mCurToken.tokenType == ID ||
             mCurToken.tokenType == LP ) {
          Expression();

          if ( mCurToken.tokenType != MRP ) {
            string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'56";
            throw ( errorMsg );
          } // end if
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'57";
          throw ( errorMsg );
        } // end else
      } // end if
    } // end if
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'58";
    throw ( errorMsg );
  } // end else
  
  // mCurToken = mScanner.GetToken();
  if ( DEBUG ) cout << "end of UnaryExp" << endl;
} // Parser::UnaryExp()

void Parser::SignedUnaryExp() {
  if ( DEBUG ) cout << "SignedUnaryExp" << endl;

  if ( mCurToken.tokenType == ID ) {
    string name = mCurToken.tokenString;
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == LP ) {
      CheckFunctionNameExist( name );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
        ActualParameterList();
      } // end if

      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'59";
        throw ( errorMsg );
      } // end if
      else {
        mCurToken = mScanner.GetToken();
      } // end else
    } // end if
    else if ( mCurToken.tokenType == MLP ) {
      FindVariable( name, true );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
      
        Expression();
        if ( mCurToken.tokenType != MRP ) {
          string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'60";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'61";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      FindVariable( name, true );
    } // end else
  } // end if
  else if ( mCurToken.tokenType == LP ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
      
      Expression();
      if ( mCurToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'62";
        throw ( errorMsg );
      } // end if
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'63";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mCurToken.tokenType == CONSTANT ) {
    mCurToken = mScanner.GetToken();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'64";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of SignedUnaryExp" << endl;
} // Parser::SignedUnaryExp()
  
void Parser::UnsignedUnaryExp() {
  if ( DEBUG ) cout << "UnsignedUnaryExp" << endl;

  if ( mCurToken.tokenType == ID ) {
    string name = mCurToken.tokenString;
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == LP ) {
      CheckFunctionNameExist( name );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if 

      if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
        ActualParameterList();
      } // end if
      else mCurToken = mScanner.GetToken();

      if ( mCurToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'65";
        throw ( errorMsg );
      } // end if
      else {
        mCurToken = mScanner.GetToken();
      } // end else
    } // end if
    else if ( mCurToken.tokenType == MLP ) {
      FindVariable( name, true );
      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
           mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
           mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
           mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
      
        Expression();
        if ( mCurToken.tokenType != MRP ) {
          string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'66";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'67";
        throw ( errorMsg );
      } // end else

      mCurToken = mScanner.GetToken();
      if ( mCurToken.tokenType == PP || mCurToken.tokenType == MM ) {
        mCurToken = mScanner.GetToken(); // do something
      } // end if
    } // end if
    else if ( mCurToken.tokenType == PP || mCurToken.tokenType == MM ) {
      mCurToken = mScanner.GetToken(); // do something
    } // end if
    else {
      FindVariable( name, true );
    } // end else
  } // end if
  else if ( mCurToken.tokenType == LP ) {
    mCurToken = mScanner.GetToken();
    if ( mCurToken.tokenType == ID || mCurToken.tokenType == PP ||
         mCurToken.tokenType == MM || mCurToken.tokenType == ADD ||
         mCurToken.tokenType == SUB || mCurToken.tokenType == BITNOT ||
         mCurToken.tokenType == CONSTANT || mCurToken.tokenType == LP ) {
      
      Expression();
      if ( mCurToken.tokenType == RP ) {
        mCurToken = mScanner.GetToken();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'68";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'69";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mCurToken.tokenType == CONSTANT ) {
    mCurToken = mScanner.GetToken();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mCurToken.tokenString + "'64";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of UnsignedUnaryExp" << endl;
} // Parser::UnsignedUnaryExp()

void Parser::StatementOp() {
  if ( mScanner.mTokenList[0].tokenString == "ListAllVariables" ) {
    ListAllVariables();
  } // end if
  else if ( mScanner.mTokenList[0].tokenString == "ListVariable" ) {
    ListVariable( mScanner.mTokenList[2].tokenString );
  } // end if
  else if ( mScanner.mTokenList[0].tokenString == "ListAllFunctions" ) {
    ListAllFunctions();
  } // end if
  else if ( mScanner.mTokenList[0].tokenString == "ListFunction" ) {
    ListFunction( mScanner.mTokenList[2].tokenString );
  } // end if
  else if ( mScanner.mTokenList[0].tokenString == "Done" ) {
    string errorMsg = "Done";
    throw ( errorMsg );
  } // end if

  cout << "Statement executed ..." << endl;
} // Parser::StatementOp()

void Parser::DefinitionOp( bool isBLP ) {
  for ( int i = 0 ; i < mDeclaratorsList.size() ; i++ ) {
    int pos = FindVariable( mDeclaratorsList[i].name, false );
    if ( pos == -1 ) { // new variable 
      Variable temp;
      temp.name = mDeclaratorsList[i].name;
      temp.type = mDeclaratorsList[i].type;
      temp.region = mRegion;
      for ( int j = 0 ; j < mDeclaratorsList[i].num ; j++ ) {
        mVariableList.push_back( temp );
      } // end for

      if ( ! isBLP ) cout << "Definition of " << temp.name << " entered ..." << endl;
    } // end if
    else { // just change type
      
      for ( int j = 0 ; j < mDeclaratorsList[i].num ; j++ ) {
        mVariableList[pos + j].type = mDeclaratorsList[i].type;
      } // end for

      if ( ! isBLP ) cout << "New definition of " << mVariableList[pos].name << " entered ..." << endl;
    } // end else
  } // end for
} // Parser::DefinitionOp()

void Parser::RemoveVariables() {
  if ( DEBUG ) cout << mRegion << " RemoveVariables" << endl;

  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( mVariableList[i].region > mRegion ) {
      mVariableList.erase( mVariableList.begin() + i );
      i--;
    } // end if
  } // end for
} // Parser::RemoveVariables()

int Parser::FindVariable( string name, bool printError ) {
  if ( DEBUG ) cout << name << " FindVariable" << endl;
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( mVariableList[i].name == name ) {
      if ( mVariableList[i].region >= mThreshold ) return i;
    } // end if
  } // end for

  if ( printError ) {
    string errorMsg = "undefined identifier : '" + name + "'79";
    throw ( errorMsg );
  } // end if

  return -1;
} // Parser::FindVariable()

int Parser::FindFunctionName( string name ) {
  if ( DEBUG ) cout << "FindFunctionName" << endl;
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( mFunctionList[i].functionName == name ) return i;
  } // end for

  return -1;
} // Parser::FindFunctionName()

void Parser::FunctionDefinitionWithoutIDOp() {
  int pos = FindFunctionName( mScanner.mTokenList[1].tokenString );
  if ( pos == -1 ) { // new function
    Function temp;
    temp.type = mScanner.mTokenList[0].tokenString;
    temp.functionName = mScanner.mTokenList[1].tokenString;
    temp.tokenList = mScanner.mTokenList;
    mFunctionList.push_back( temp );
      
    cout << "Definition of " << temp.functionName << "() entered ..." << endl;
  } // end if
  else { // just change type
    mFunctionList[pos].type = mScanner.mTokenList[0].tokenString;
    mFunctionList[pos].tokenList = mScanner.mTokenList;
    cout << "New definition of " << mFunctionList[pos].functionName << "() entered ..." << endl;
  } // end else
} // Parser::FunctionDefinitionWithoutIDOp()

void Parser::CheckFunctionNameExist( string name ) {
  if ( name == "ListAllFunctions" || name == "ListAllVariables" ||
       name == "ListVariable" || name == "ListFunction" || 
       name == "Done" ) {

    return;
  } // end if

  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( mFunctionList[i].functionName == name ) return;
  } // end for

  string errorMsg = "undefined identifier : '" + name + "'79";
  throw ( errorMsg );
} // Parser::CheckFunctionNameExist()

void Parser::ListAllFunctions() {
  FunctionSort();

  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    cout << mFunctionList[i].functionName << "()" << endl;
  } // end for

} // Parser::ListAllFunctions()

void Parser::ListAllVariables() {
  VariableSort();
  
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( i == 0 || mVariableList[i].name != mVariableList[i - 1].name ) {
      cout << mVariableList[i].name << endl;
    } // end if
  } // end for

} // Parser::ListAllVariables()

void Parser::ListFunction( string name ) {
  name = name.substr( 1, name.size() - 2 );
  int preSpace = 0;
  string pre = "";
  string cur = "";
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( name == mFunctionList[i].functionName ) {
      cout << mFunctionList[i].type << " " << mFunctionList[i].functionName;
      for ( int j = 2 ; j < mFunctionList[i].tokenList.size() ; j++ ) {
        cout << mFunctionList[i].tokenList[j].tokenString;
        if ( j == mFunctionList[i].tokenList.size() - 2 ) cout << endl;
        else if ( j == mFunctionList[i].tokenList.size() - 1 ) ;
        else if ( mFunctionList[i].tokenList[j].tokenString == ";" ) {
          cout << endl;
          if ( j < mFunctionList[i].tokenList.size() - 1 && 
               mFunctionList[i].tokenList[j + 1].tokenString == "}" ) preSpace = preSpace - 2;
          for ( int k = 0 ; k < preSpace ; k++ ) {
            cout << " ";
          } // end for
        } // end if
        else if ( mFunctionList[i].tokenList[j].tokenString == "{" ) {
          cout << endl;
          preSpace = preSpace + 2;
          for ( int k = 0 ; k < preSpace ; k++ ) {
            cout << " ";
          } // end for
        } // end if
        else if ( mFunctionList[i].tokenList[j].tokenString == "}" ) {
          cout << endl;
          
          for ( int k = 0 ; k < preSpace ; k++ ) {
            cout << " ";
          } // end for

        } // end if
        else if ( mFunctionList[i].tokenList[j + 1].tokenString != "[" &&
                  ( mFunctionList[i].tokenList[j + 1].tokenString != "(" ||
                    ( mFunctionList[i].tokenList[j].tokenString == "while" || 
                      mFunctionList[i].tokenList[j].tokenString == "if" ) ) &&
                  mFunctionList[i].tokenList[j + 1].tokenString != "++" ) cout << " ";
      } // end for

      cout << endl;
      return;
    } // end if
  } // end for


} // Parser::ListFunction()

void Parser::ListVariable( string name ) {
  name = name.substr( 1, name.size() - 2 );
  int count = 0;
  string type = "";

  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    if ( mVariableList[i].name == name ) {
      if ( type == "" ) type = mVariableList[i].type;
      if ( type == mVariableList[i].type ) count++;
    } // end if
  } // end for

  if ( count > 0 ) cout << type << " " << name;
  if ( count > 1 ) {
    cout << "[ " << IntToString( count ) << " ]";
  } // end if

  if ( count > 0 ) cout << " ;" << endl;
} // Parser::ListVariable()

void Parser::VariableSort() {
  for ( int i = 0 ; i < mVariableList.size() ; i++ ) {
    for ( int j = i ; j < mVariableList.size() ; j++ ) {
      if ( VariableCompare( mVariableList[i], mVariableList[j] ) ) {
        Variable temp = mVariableList[i];
        mVariableList[i] = mVariableList[j];
        mVariableList[j] = temp;
      } // end if
    } // end for
  } // end for
} // Parser::VariableSort()

void Parser::FunctionSort() {
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    for ( int j = i ; j < mFunctionList.size() ; j++ ) {
      if ( FunctionCompare( mFunctionList[i], mFunctionList[j] ) ) {
        Function temp = mFunctionList[i];
        mFunctionList[i] = mFunctionList[j];
        mFunctionList[j] = temp;
      } // end if
    } // end for
  } // end for
} // Parser::FunctionSort()

bool Parser::FindElse( int findIf ) {
  int ifFind, elseFind;
  ifFind = elseFind = 0;
  for ( int i = findIf ; i < mScanner.mTokenList.size() ; i++ ) {
    if ( mScanner.mTokenList[i].tokenString == "else" ) elseFind++;
    if ( mScanner.mTokenList[i].tokenString == "if" ) ifFind++;
  } // end for

  return ( ifFind - elseFind );
} // Parser::FindElse()

bool Parser::JustIfStatement() {
  int findIf, findElse;
  findIf = findElse = 0;
  for ( int i = 0 ; i < mScanner.mTokenList.size() ; i++ ) {
    if ( mScanner.mTokenList[i].tokenString == "else" ) findElse++;
    if ( mScanner.mTokenList[i].tokenString == "if" ) findIf++;
  } // end for

  return ( findIf - findElse );
} // Parser::JustIfStatement()

int main() {
  int uTestNum = 0 ;
  char ch;
  Parser parser; 
  bool get = true;
  if ( ! DEBUG ) scanf( "%d%c", &uTestNum, &ch );
  
  printf( "Our-C running ...\n" ) ;
  printf( "> " );
  
  parser.mScanner.Init();
  parser.mScanner.CurInit();
  parser.Init();
  while ( parser.IsUserInput() ) {
    printf( "> " );
  } // while
  
  printf( "Our-C exited ..." ) ;
} // main()
