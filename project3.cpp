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
  NONE, END_OF_FILE, COMMENT  // ��l��
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
  return a.name > b.name; // ���ǱƦC
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
    else if ( ( mCur >= '0' && mCur <= '9' ) || mCur == '.' ) {
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
      mToken.lineNum = mCurLine;
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
  else if ( mToken.tokenString == "true" ) {
    mToken.tokenType = CONSTANT;
  } // end if
  else if ( mToken.tokenString == "false" ) {
    mToken.tokenType = CONSTANT;
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
  Token mToken;
  bool mExecute;
  int mCur;
  vector<Variable> mVariableList;
  vector<Function> mFunctionList;
  vector<Declarators> mDeclaratorsList;

  
  void ListFunction( string name ) ;
  void ListVariable( string name ) ;
  void ListAllVariables() ;
  void ListAllFunctions() ;
  
  bool IsDefOrStatement() ;
  void UserInput() ;
  void GetFirstToken() ;
  void Definition( bool isBLP ) ;
  void TypeSpecifier() ;
  void FunctionDefinitionOrDeclarators( string type, string name, bool isBLP ) ;
  void RestOfDeclarators( string variableType, string variableName, bool isBLP ) ;
  void FunctionDefinitionWithoutID() ;
  void FormalParameterList() ;
  void Declaration( bool isBLP ) ;

  void CompoundStatement() ;
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

  void CompoundStatementOp( string &result ) ;
  void StatementOp( string &result ) ;
  void StatementWhileOp( string &result ) ;
  bool StatementIfOp( string &result ) ;
  void ExpressionOp( string &result ) ;
  void CoutExpressionOp( string &result ) ;
  void CinExpressionOp( string &result ) ;
  void BasicExpressionOp( string &result ) ;
  void RestOfIDStartedBasicExpOp( string name, string &result ) ;
  void RestOfPPMMIDStartedBasicExpOp( string &result ) ;
  void SignOp( string &result ) ;
  void ActualParameterListOp( string &result ) ;
  void AssignmentOperatorOp( string &result ) ;
  void RomceAndRomloeOp( string &result ) ;
  void RestOfMaybeLogicalOrExpOp( string &result ) ;
  void MaybeLogicalAndExpOp( string &result ) ;
  void RestOfMaybeLogicalAndExpOp( string &result ) ;
  void MaybeBitOrExpOp( string &result ) ;
  void RestOfMaybeBitOrExpOp( string &result ) ;
  void MaybeBitExOrExpOp( string &result ) ;
  void RestOfMaybeBitExOrExpOp( string &result ) ;
  void MaybeBitAndExpOp( string &result ) ;
  void RestOfMaybeBitAndExpOp( string &result ) ;
  void MaybeEqualityExpOp( string &result ) ;
  void RestOfMaybeEqualityExpOp( string &result ) ;
  void MaybeRelationalExpOp( string &result ) ;
  void RestOfMaybeRelationalExpOp( string &result ) ;
  void MaybeShiftExpOp( string &result ) ;
  void RestOfMaybeShiftExpOp( string &result ) ;
  void MaybeAdditiveExpOp( string &result ) ;
  void RestOfMaybeAdditiveExpOp( string &result ) ;
  void MaybeMultExpOp( string &result ) ;
  void RestOfMaybeMultExpOp( string &result ) ;
  void UnaryExpOp( string &result ) ;
  void SignedUnaryExpOp( string &result ) ;
  void UnsignedUnaryExpOp( string &result ) ;
}; // Parser

void Parser::Init() {
  bool end = false;
  mVariableList.clear();
  mFunctionList.clear();
  mDeclaratorsList.clear();
  mScanner.mCurLine = 1;
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
      msg = msg.substr( 0, msg.length() - 2 );
      if ( mScanner.mToken.lineNum == 0 ) {
        mScanner.mToken.lineNum = 1;
        mScanner.mCurLine = 1;
      } // end if

      cout << "Line " << mScanner.mToken.lineNum << " : " << msg << endl;
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
      msg = msg.substr( 0, msg.length() - 2 );
      if ( mScanner.mToken.lineNum == 0 ) {
        mScanner.mToken.lineNum = 1;
      } // end if

      cout << "Line " << mScanner.mToken.lineNum << " : " << msg << endl;
      mScanner.LineInit();
      mScanner.SkipErrorLine();
      mScanner.mCurLine++;
      mScanner.GetNextChar();
    } // catch
  } // end while
} // Parser::GetFirstToken()

bool Parser::IsUserInput() {
  mDeclaratorsList.clear();
  try {
    mLastStatementIf = false;
    mExecute = true;
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
    else cout << "Line " << mScanner.mTokenList[mScanner.mTokenList.size() - 1].lineNum << " : ";

    if ( ! DEBUG ) msg = msg.substr( 0, msg.length() - 2 );
    cout << msg << endl;
    mRegion = 0;
    RemoveVariables();

    printf( "> " );

    if ( num == "79" ) {
      if ( mScanner.mTokenList[mScanner.mTokenList.size() - 2].lineNum == mToken.lineNum ) {
        mScanner.SkipErrorLine();
        GetFirstToken();
      } // end if
      else { // hold the token
        mToken.lineNum = mToken.lineNum - 
                            mScanner.mTokenList[mScanner.mTokenList.size() - 2].lineNum;
        mScanner.mToken.lineNum = mScanner.mCurLine = mToken.lineNum;
        mScanner.mTokenList.clear();
        mScanner.mTokenList.push_back( mToken );
      } // end else

      return true;
    } // end if

    mScanner.SkipErrorLine();
    GetFirstToken();
    return true;
  } // catch

  if ( mJustPeek ) { // hold the token
    mScanner.mTokenList.clear();
    mToken.tokenString = mScanner.mToken.tokenString;
    mToken.tokenType = mScanner.mToken.tokenType;
    mToken.lineNum = mScanner.mToken.lineNum - mToken.lineNum;
    mScanner.mCurLine = mToken.lineNum;
    mScanner.mTokenList.push_back( mToken );
    mRegion = 0;
    RemoveVariables();
    printf( "> " );
    return true;
  } // end if

  printf( "> " );
  GetFirstToken();
  mRegion = 0;
  RemoveVariables();
    

  if ( DEBUG ) cout << "The grammer is correct" << endl;
  return true;
} // Parser::IsUserInput()

void Parser::UserInput() {
  // Token curToken = mScanner.GetMemberToken();
  if ( DEBUG ) cout << "UserInput" << endl;

  
  
  if ( mToken.tokenType == END_OF_FILE ) {
    throw ( "EOF" );
  } // end if
  else if ( mToken.tokenType == VOID || 
            mToken.tokenType == INT || 
            mToken.tokenType == CHAR || 
            mToken.tokenType == FLOAT || 
            mToken.tokenType == STRING || 
            mToken.tokenType == BOOL ) { // the first type of definition
    Definition( false );
  } // end if
  else if ( mToken.tokenType == ID ||
            mToken.tokenType == PP ||
            mToken.tokenType == MM ||
            mToken.tokenType == ADD ||
            mToken.tokenType == SUB ||
            mToken.tokenType == BITNOT ||
            mToken.tokenType == CONSTANT ||
            mToken.tokenType == RETURN ||
            mToken.tokenType == BLP ||
            mToken.tokenType == IF || 
            mToken.tokenType == WHILE ||
            mToken.tokenType == DO ||
            mToken.tokenType == SCLN ||
            mToken.tokenType == LP ||
            mToken.tokenType == CIN ||
            mToken.tokenType == COUT ) {
    Statement();
    StatementOp();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'01";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of UserInput" << endl; 
} // Parser::UserInput()

void Parser::Definition( bool isBLP ) {
  if ( DEBUG ) cout << "Definition" << endl;
  if ( mToken.tokenType == VOID ) { // should be a function
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mToken.tokenType == ID ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == LP ) FunctionDefinitionWithoutID(); 
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'02";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'02";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mToken.tokenType == INT || 
            mToken.tokenType == CHAR || 
            mToken.tokenType == FLOAT || 
            mToken.tokenType == STRING || 
            mToken.tokenType == BOOL ) {
              
    string type = mToken.tokenString;
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mToken.tokenType == ID ) {
      FunctionDefinitionOrDeclarators( type, mToken.tokenString, isBLP ); 
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'03";
      throw ( errorMsg );
    } // end else
    
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'04";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of Definition" << endl;
} // Parser::Definition()

void Parser::TypeSpecifier() {
  if ( DEBUG ) cout << "TypeSpecifier" << endl;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE )  {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mToken.tokenType != INT && mToken.tokenType != CHAR && mToken.tokenType != FLOAT &&
            mToken.tokenType != STRING && mToken.tokenType != BOOL ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'05";
    throw ( errorMsg );
  } // end if
  
  if ( DEBUG ) cout << "end of TypeSpecifier" << endl;
} // Parser::TypeSpecifier()

void Parser::FunctionDefinitionOrDeclarators( string type, string name, bool isBLP ) {
  if ( DEBUG ) cout << "FunctionDefinitionOrDeclarators" << endl;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mToken.tokenType == LP ) {
    
    // record the type and name of the function
    
    FunctionDefinitionWithoutID();
  } // end if
  else if ( mToken.tokenType == MLP || mToken.tokenType == SCLN || mToken.tokenType == COMMA ) {
    RestOfDeclarators( type, name, isBLP );
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'06";
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
  if ( mToken.tokenType == MLP ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mToken.tokenType == CONSTANT ) {
      string length = mToken.tokenString;
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mToken.tokenType != MRP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'07";
        throw ( errorMsg );
      } // end if
      else if ( mToken.tokenType == MRP ) {
        mToken = mScanner.GetToken();
        mDeclaratorsList[0].num = StringToInt( length );
      } // end if
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'08";
      throw ( errorMsg );
    } // end else
  } // end if

  if ( mToken.tokenType == COMMA ) {
    while ( mToken.tokenType == COMMA ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mToken.tokenType == ID ) { // check is there any '[' CONSTANT ']' pattern next
        temp.name = mToken.tokenString;
        mToken = mScanner.GetToken();
        if ( mToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if
        else if ( mToken.tokenType == MLP ) { // check '['
          mToken = mScanner.GetToken();
          if ( mToken.tokenType == END_OF_FILE ) {
            string error = "EOF";
            throw ( error );
          } // end if
          else if ( mToken.tokenType == CONSTANT ) {  // check CONSTANT
            string length = mToken.tokenString;
            mToken = mScanner.GetToken();
            if ( mToken.tokenType == END_OF_FILE ) {
              string error = "EOF";
              throw ( error );
            } // end if
            else if ( mToken.tokenType != MRP ) {  // check ']'
              string errorMsg = "unexpected token : '" + mToken.tokenString + "'09";
              throw ( errorMsg );
            } // end if
            else { // the pattern is correct
              temp.num = StringToInt( length );
              temp.type = variableType;
              mDeclaratorsList.push_back( temp );
              mToken = mScanner.GetToken();
            } // end else
          } // end if
          else {
            string errorMsg = "unexpected token : '" + mToken.tokenString + "'10";
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
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'11";
        throw ( errorMsg );
      } // end else
    } // end while
  } // end if
  
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mToken.tokenType != SCLN ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'12";
    throw ( errorMsg );
  } // end if
  
  DefinitionOp( isBLP );

  if ( DEBUG ) cout << "end of RestOfDeclarators" << endl;
} // Parser::RestOfDeclarators()

void Parser::FunctionDefinitionWithoutID() {
  if ( DEBUG ) cout << "FunctionDefinitionWithoutID" << endl;
  mRegion++;
  mThreshold = mRegion;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mToken.tokenType == INT || 
            mToken.tokenType == CHAR || 
            mToken.tokenType == FLOAT || 
            mToken.tokenType == STRING || 
            mToken.tokenType == BOOL ) {
    FormalParameterList();
  } // end if

  if ( mToken.tokenType == RP ) mToken = mScanner.GetToken();
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'14";
    throw ( errorMsg );
  } // end else

  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mToken.tokenType == BLP ) {
    CompoundStatement();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'14";
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
  temp.type = mToken.tokenString;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  
  if ( mToken.tokenType == BITAND ) { // check for '&'
    mToken = mScanner.GetToken();
  } // end if
  
  if ( mToken.tokenType != ID ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'15";
    throw ( errorMsg );
  } // end if
  else {
    temp.name = mToken.tokenString;
  } // end else

  mToken = mScanner.GetToken();
  if ( mToken.tokenType == MLP ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mToken.tokenType == CONSTANT ) {
      temp.num = StringToInt( mToken.tokenString );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mToken.tokenType != MRP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'16";
        throw ( errorMsg );
      } // end if
      else mToken = mScanner.GetToken();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'17";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    temp.num = 1;
  } // end else

  mDeclaratorsList.push_back( temp );

  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  while ( mToken.tokenType == COMMA ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mToken.tokenType != INT && 
         mToken.tokenType != CHAR && 
         mToken.tokenType != FLOAT && 
         mToken.tokenType != STRING && 
         mToken.tokenType != BOOL ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'18";
      throw ( errorMsg );
    } // end if
    else {
      temp.type = mToken.tokenString;
    } // end else
           
    mToken = mScanner.GetToken();
  
    if ( mToken.tokenType == BITAND ) { // check for '&' (optional)
      mToken = mScanner.GetToken();
    } // end if
      
      
      
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    else if ( mToken.tokenType == ID ) { // check is there any '[' CONSTANT ']' pattern next
      temp.name = mToken.tokenString;
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      else if ( mToken.tokenType == MLP ) { // check '['
        mToken = mScanner.GetToken();
        if ( mToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if
        else if ( mToken.tokenType == CONSTANT ) {  // check CONSTANT
          temp.num = StringToInt( mToken.tokenString );
          mToken = mScanner.GetToken();
          if ( mToken.tokenType == END_OF_FILE ) {
            string error = "EOF";
            throw ( error );
          } // end if
          else if ( mToken.tokenType != MRP ) {  // check ']'
            string errorMsg = "unexpected token : '" + mToken.tokenString + "'19";
            throw ( errorMsg );
          } // end if
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'20";
          throw ( errorMsg );
        } // end else
      } // end if
      else {
        temp.num = 1;
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'21";
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
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( IsDefOrStatement() ) {
    while ( IsDefOrStatement() ) {
           
      if ( mToken.tokenType == INT || 
           mToken.tokenType == CHAR || 
           mToken.tokenType == FLOAT || 
           mToken.tokenType == STRING || 
           mToken.tokenType == BOOL ) {
        Declaration( true );
      } // end if
      else if ( mToken.tokenType == ID ||
                mToken.tokenType == PP ||
                mToken.tokenType == MM ||
                mToken.tokenType == ADD ||
                mToken.tokenType == SUB ||
                mToken.tokenType == BITNOT ||
                mToken.tokenType == CONSTANT ||
                mToken.tokenType == RETURN ||
                mToken.tokenType == BLP ||
                mToken.tokenType == IF || 
                mToken.tokenType == WHILE ||
                mToken.tokenType == DO ||
                mToken.tokenType == SCLN ||
                mToken.tokenType == LP ||
                mToken.tokenType == CIN ||
                mToken.tokenType == COUT ) {

        Statement();
      } // end if
      
      if ( ! mJustPeek ) mToken = mScanner.GetToken();
      else {
        mToken = mScanner.mToken;
        mJustPeek = false;
      } // end else

      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
    } // end while
  } // end if
  
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mToken.tokenType != BRP ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'23";
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
  string type = mToken.tokenString;
  string name;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( mToken.tokenType == ID ) {
    name = mToken.tokenString;
    mToken = mScanner.GetToken();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'24";
    throw ( errorMsg );
  } // end else
  
  if ( mToken.tokenType == MLP || mToken.tokenType == SCLN || mToken.tokenType == COMMA ) {
    RestOfDeclarators( type, name, isBLP );
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'25";
    throw ( errorMsg );
  } // end else
  
  if ( DEBUG ) cout << "end of Declaration" << endl;
} // Parser::Declaration()

void Parser::Statement() {
  if ( DEBUG ) cout << "Statement" << endl;
  mLastStatementIf = false;
  
  if ( DEBUG ) cout << mToken.tokenString << mIfStatement << endl;
  
  if ( mToken.tokenType == SCLN ) return; // condition 1
  else if ( mToken.tokenType == ID ||
            mToken.tokenType == PP ||
            mToken.tokenType == MM ||
            mToken.tokenType == ADD ||
            mToken.tokenType == SUB ||
            mToken.tokenType == BITNOT ||
            mToken.tokenType == CONSTANT ||
            mToken.tokenType == LP ) { // condition 2

    Expression();
    
    if ( mToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'26";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mToken.tokenType == RETURN ) { // condition 3
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mToken.tokenType == ID ||
         mToken.tokenType == PP ||
         mToken.tokenType == MM ||
         mToken.tokenType == ADD ||
         mToken.tokenType == SUB ||
         mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT ||
         mToken.tokenType == LP ) {
      
      Expression();
    } // end if
    
    if ( mToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'27";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mToken.tokenType == BLP ) { // condition 4
    CompoundStatement();
  } // end if
  else if ( mToken.tokenType == IF ) { // condition 5
    mLastStatementIf = StatementIf();
  } // end if
  else if ( mToken.tokenType == WHILE ) { // condition 6
    StatementWhile();
  } // end if
  else if ( mToken.tokenType == DO ) { // condition 7
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    Statement();

    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mToken.tokenType == WHILE ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mToken.tokenType == LP ) {
        mToken = mScanner.GetToken();
        if ( mToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if

        if ( mToken.tokenType == ID ||
             mToken.tokenType == PP ||
             mToken.tokenType == MM ||
             mToken.tokenType == ADD ||
             mToken.tokenType == SUB ||
             mToken.tokenType == BITNOT ||
             mToken.tokenType == CONSTANT ||
             mToken.tokenType == ID ||
             mToken.tokenType == LP ) {
          Expression();
        } // end if
        else if ( mToken.tokenType == COUT ) {
          CoutExpression();
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'34";
          throw ( errorMsg );
        } // end else
    
        if ( mToken.tokenType != RP ) {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'35";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'29";
        throw ( errorMsg );
      } // end else
      
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mToken.tokenType != SCLN ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'30";
        throw ( errorMsg );
      } // end if
    } // end if
  } // end if
  else if ( mToken.tokenType == ELSE && mIfStatement ) {
    mIfStatement--;
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    Statement();
  } // end if
  else if ( mToken.tokenType == COUT ) {
    CoutExpression();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'30";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mToken.tokenType == CIN ) {
    CinExpression();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'3'";
    throw ( errorMsg );
  } // end else
  
  if ( DEBUG ) cout << "end of Statement" << endl;
} // Parser::Statement()

void Parser::CoutExpression() {
  if ( DEBUG ) cout << "CoutExpression" << endl;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if

  if ( mToken.tokenType != LS ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
    throw ( errorMsg );
  } // end if

  while ( mToken.tokenType == LS ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mToken.tokenType == ID ||
         mToken.tokenType == PP ||
         mToken.tokenType == MM ||
         mToken.tokenType == ADD ||
         mToken.tokenType == SUB ||
         mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT ||
         mToken.tokenType == LP ) { // condition 2

      Expression();

    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
      throw ( errorMsg );
    } // end else
  } // end while
    
} // Parser::CoutExpression()

void Parser::CinExpression() {
  if ( DEBUG ) cout << "CinExpression" << endl;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if

  if ( mToken.tokenType == RS ) {
    while ( mToken.tokenType == RS ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mToken.tokenType == ID || mToken.tokenType == LP || mToken.tokenType == CONSTANT ) {
        UnsignedUnaryExp() ;
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
        throw ( errorMsg );
      } // end else
    } // end while

    if ( mToken.tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'35";
      throw ( errorMsg );
    } // end if
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'36";
    throw ( errorMsg );
  } // end else
} // Parser::CinExpression()

bool Parser::StatementIf() {
  if ( DEBUG ) cout << " StatementIf" << endl;
  mToken = mScanner.GetToken();
  mIfStatement++;

  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
    
  if ( mToken.tokenType == LP ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mToken.tokenType == ID ||
         mToken.tokenType == PP ||
         mToken.tokenType == MM ||
         mToken.tokenType == ADD ||
         mToken.tokenType == SUB ||
         mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT || 
         mToken.tokenType == LP ) {
      Expression();
    } // end if
    else if ( mToken.tokenType == COUT ) {
      CoutExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'31";
      throw ( errorMsg );
    } // end else
    
    if ( mToken.tokenType != RP ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'32";
      throw ( errorMsg );
    } // end if
    
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mToken.tokenType == ID ||
         mToken.tokenType == PP ||
         mToken.tokenType == MM ||
         mToken.tokenType == ADD ||
         mToken.tokenType == SUB ||
         mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT ||
         mToken.tokenType == RETURN ||
         mToken.tokenType == BLP ||
         mToken.tokenType == IF || 
         mToken.tokenType == WHILE ||
         mToken.tokenType == DO ||
         mToken.tokenType == SCLN ||
         mToken.tokenType == CIN ||
         mToken.tokenType == COUT ||
         mToken.tokenType == LP ) {
      Statement();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
      throw ( errorMsg );
    } // end else
    
    
    mScanner.GetToken();

    if ( mScanner.mToken.tokenType == ELSE ) {
      mToken = mScanner.mToken;
      Statement();
      return false;
    } // end if
    else {
      
      mJustPeek = true;
    } // end else
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'31";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << " end of StatementIf" << endl;

  return true;
} // Parser::StatementIf()

void Parser::StatementWhile() {
  if ( DEBUG ) cout << "StatementWhile" << endl;
  mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
    
  if ( mToken.tokenType == LP ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mToken.tokenType == ID ||
         mToken.tokenType == PP ||
         mToken.tokenType == MM ||
         mToken.tokenType == ADD ||
         mToken.tokenType == SUB ||
         mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT ||
         mToken.tokenType == ID ||
         mToken.tokenType == LP ) {
      Expression();
    } // end if
    else if ( mToken.tokenType == COUT ) {
      CoutExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'34";
      throw ( errorMsg );
    } // end else
    
    if ( mToken.tokenType != RP ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'35";
      throw ( errorMsg );
    } // end if
    
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mToken.tokenType == ID ||
         mToken.tokenType == PP ||
         mToken.tokenType == MM ||
         mToken.tokenType == ADD ||
         mToken.tokenType == SUB ||
         mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT ||
         mToken.tokenType == RETURN ||
         mToken.tokenType == BLP ||
         mToken.tokenType == IF || 
         mToken.tokenType == WHILE ||
         mToken.tokenType == DO ||
         mToken.tokenType == SCLN ||
         mToken.tokenType == CIN ||
         mToken.tokenType == COUT ||
         mToken.tokenType == LP ) {
      Statement();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'36";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'31";
    throw ( errorMsg );
  } // end else
  
  if ( DEBUG ) cout << "end of StatementWhile" << endl;
} // Parser::StatementWhile()

bool Parser::IsDefOrStatement() {
  if ( DEBUG ) cout << "IsDefOrStatement" << endl;
  return ( mToken.tokenType == INT || 
           mToken.tokenType == CHAR || 
           mToken.tokenType == FLOAT || 
           mToken.tokenType == STRING || 
           mToken.tokenType == BOOL ||
           mToken.tokenType == ID ||
           mToken.tokenType == PP ||
           mToken.tokenType == MM ||
           mToken.tokenType == ADD ||
           mToken.tokenType == SUB ||
           mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT ||
           mToken.tokenType == RETURN ||
           mToken.tokenType == BLP ||
           mToken.tokenType == IF || 
           mToken.tokenType == WHILE ||
           mToken.tokenType == DO ||
           mToken.tokenType == SCLN ||
           mToken.tokenType == LP ||
           mToken.tokenType == CIN ||
           mToken.tokenType == COUT );
} // Parser::IsDefOrStatement()

void Parser::Expression() {
  if ( DEBUG ) cout << "Expression" << endl;
  BasicExpression();
  
  if ( mToken.tokenType == COMMA ) {
    while ( mToken.tokenType == COMMA ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mToken.tokenType == ID ||
           mToken.tokenType == PP ||
           mToken.tokenType == MM ||
           mToken.tokenType == ADD ||
           mToken.tokenType == SUB ||
           mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT ||
           mToken.tokenType == LP ) {
        BasicExpression();
      } // end if
      
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    } // end while
  } // end if
  
  if ( DEBUG ) cout << "end of Expression" << endl;
} // Parser::Expression()

void Parser::BasicExpression() {
  if ( DEBUG ) cout << "BasicExpression" << endl;
  if ( mToken.tokenType == ID ) {
    // FindVariable( mToken.tokenString, true );
    string name = mToken.tokenString;
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) { 
      string error = "EOF";
      throw ( error );
    } // end if
    
    RestOfIDStartedBasicExp( name );
  } // end if
  else if ( mToken.tokenType == PP || mToken.tokenType == MM ) { // condition 2
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mToken.tokenType == ID ) {
      FindVariable( mToken.tokenString, true );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      RestOfPPMMIDStartedBasicExp();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'38";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mToken.tokenType == ADD || mToken.tokenType == SUB || 
            mToken.tokenType == BITNOT ) { // condition 3
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    while ( mToken.tokenType == ADD || mToken.tokenType == SUB || mToken.tokenType == BITNOT ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    } // end while
    
    if ( mToken.tokenType == ID || mToken.tokenType == CONSTANT || 
         mToken.tokenType == LP ) {
      SignedUnaryExp();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'39";
      throw ( errorMsg );
    } // end else
  
    RomceAndRomloe();
  } // end if
  else if ( mToken.tokenType == CONSTANT || mToken.tokenType == LP ) { // condition 4
    if ( mToken.tokenType == LP ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    
      if ( mToken.tokenType == ID ||
           mToken.tokenType == PP ||
           mToken.tokenType == MM ||
           mToken.tokenType == ADD ||
           mToken.tokenType == SUB ||
           mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT ||
           mToken.tokenType == ID ||
           mToken.tokenType == LP ) {
        Expression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'40";
        throw ( errorMsg );
      } // end else
      
      if ( mToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'41";
        throw ( errorMsg );      
      } // end if
    } // end if
  
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if	

    RomceAndRomloe();
  } // end if
  
  if ( DEBUG ) cout << "end of BasicExpression" << endl;
} // Parser::BasicExpression()

void Parser::RestOfIDStartedBasicExp( string name ) {
  if ( DEBUG ) cout << "RestOfIDStartedBasicExp" << endl;
  if ( mToken.tokenType == MLP || mToken.tokenType == ASSIGN || 
       mToken.tokenType == TE || mToken.tokenType == DE || 
       mToken.tokenType == RE || mToken.tokenType == PE || 
       mToken.tokenType == ME || mToken.tokenType == PP ||
       mToken.tokenType == MM || mToken.tokenType == ADD || 
       mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
       mToken.tokenType == ID || mToken.tokenType == CONSTANT ) { // condition 1

    if ( mToken.tokenType == MLP ) {
      FindVariable( name, true );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    
      if ( mToken.tokenType == ID ||
           mToken.tokenType == PP ||
           mToken.tokenType == MM ||
           mToken.tokenType == ADD ||
           mToken.tokenType == SUB ||
           mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT ||
           mToken.tokenType == ID ||
           mToken.tokenType == LP ) {
        Expression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'42";
        throw ( errorMsg );
      } // end else
    
      if ( mToken.tokenType != MRP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'43";
        throw ( errorMsg );
      } // end if
      else {
        mToken = mScanner.GetToken();
      } // end else
    } // end if
    
    if ( mToken.tokenType == ASSIGN || mToken.tokenType == TE || 
         mToken.tokenType == DE || mToken.tokenType == RE || 
         mToken.tokenType == PE || mToken.tokenType == ME ) {

      FindVariable( name, true );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mToken.tokenType == ID || mToken.tokenType == PP ||
           mToken.tokenType == MM || mToken.tokenType == ADD ||
           mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
        BasicExpression();
      } // end if
    } // end if
    else  {
      if ( mToken.tokenType == MM || mToken.tokenType == PP ) mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      FindVariable( name, true );
      RomceAndRomloe();
    } // end else
  } // end if
  else if ( mToken.tokenType == LP ) {
    CheckFunctionNameExist( name );
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mToken.tokenType == ID || mToken.tokenType == PP ||
         mToken.tokenType == MM || mToken.tokenType == ADD ||
         mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
      ActualParameterList();
      
    } // end if

    if ( mToken.tokenType == RP ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      RomceAndRomloe();
      /*
      else  {
        string errorMsg = "unexpected token '" + mToken.tokenString + "'\n45";
        throw ( errorMsg );
      } // end else
      */
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'46";
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
  if ( mToken.tokenType == MLP ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mToken.tokenType == ID ||
         mToken.tokenType == PP ||
         mToken.tokenType == MM ||
         mToken.tokenType == ADD ||
         mToken.tokenType == SUB ||
         mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT ||
         mToken.tokenType == ID ||
         mToken.tokenType == LP ) {
      Expression();
      
      
      if ( mToken.tokenType == MRP ) {
        mToken = mScanner.GetToken();
        if ( mToken.tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if
        
        if ( mToken.tokenType == PP || mToken.tokenType == LP ||
             mToken.tokenType == MM || mToken.tokenType == ADD || 
             mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
             mToken.tokenType == ID || mToken.tokenType == CONSTANT ) {

          RomceAndRomloe();
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'47";
          throw ( errorMsg );
        } // end else
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'48";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'49";
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
  if ( mToken.tokenType != ADD && mToken.tokenType != SUB && mToken.tokenType != BITNOT ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'50";
    throw ( errorMsg );
  } // end if
} // Parser::Sign()

void Parser::ActualParameterList() {
  if ( DEBUG ) cout << "ActualParameterList" << endl;
  BasicExpression();
  
  while ( mToken.tokenType == COMMA ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mToken.tokenType == ID || mToken.tokenType == PP ||
         mToken.tokenType == MM || mToken.tokenType == ADD ||
         mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
      BasicExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'51";
      throw ( errorMsg );
    } // end else
    

  } // end while
  
  if ( DEBUG ) cout << "end of ActualParameterList" << endl;
} // Parser::ActualParameterList()

void Parser::AssignmentOperator() {
  if ( DEBUG ) cout << "AssignmentOperator" << endl;
  if ( mToken.tokenType != ASSIGN && mToken.tokenType != TE && 
       mToken.tokenType != DE && mToken.tokenType != RE &&
       mToken.tokenType != PE && mToken.tokenType != ME ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'52";
    throw ( errorMsg );
  } // end if
} // Parser::AssignmentOperator()

void Parser::RomceAndRomloe() {
  if ( DEBUG ) cout << "RomceAndRomloe" << endl;
  
  RestOfMaybeLogicalOrExp();
  
  // mToken = mScanner.GetToken();
  if ( mToken.tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( mToken.tokenType == QUE ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
  
    if ( mToken.tokenType == ID || mToken.tokenType == PP ||
         mToken.tokenType == MM || mToken.tokenType == ADD ||
         mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
      BasicExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'53";
      throw ( errorMsg );
    } // end else
    
    if ( mToken.tokenType == COLON ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mToken.tokenType == ID || mToken.tokenType == PP ||
           mToken.tokenType == MM || mToken.tokenType == ADD ||
           mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
        BasicExpression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'54";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'55";
      throw ( errorMsg );
    } // end else
  } // end if
  
  if ( DEBUG ) cout << "end of RomceAndRomloe" << endl;
} // Parser::RomceAndRomloe()

void Parser::RestOfMaybeLogicalOrExp() {
  if ( DEBUG ) cout << "RestOfMaybeLogicalOrExp" << endl;
  RestOfMaybeLogicalAndExp();
  
  
  while ( mToken.tokenType == OR ) {
    mToken = mScanner.GetToken();
    
    MaybeLogicalAndExp();
  } // end while

  if ( DEBUG ) cout << "end of RestOfMaybeLogicalOrExp" << endl;
} // Parser::RestOfMaybeLogicalOrExp()

void Parser::MaybeLogicalAndExp() {
  if ( DEBUG ) cout << "MaybeLogicalAndExp" << endl;
  MaybeBitOrExp();
  
  
  while ( mToken.tokenType == AND ) {
    mToken = mScanner.GetToken();
    
    MaybeBitOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeLogicalAndExp" << endl;
} // Parser::MaybeLogicalAndExp()

void Parser::RestOfMaybeLogicalAndExp() {
  if ( DEBUG ) cout << "RestOfMaybeLogicalAndExp" << endl;
  RestOfMaybeBitOrExp();
  
  while ( mToken.tokenType == AND ) {
    mToken = mScanner.GetToken();
    
    MaybeBitOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeLogicalAndExp" << endl;
} // Parser::RestOfMaybeLogicalAndExp()

void Parser::MaybeBitOrExp() {
  if ( DEBUG ) cout << "MaybeBitOrExp" << endl;
  MaybeBitExOrExp();
  
  while ( mToken.tokenType == BITOR ) {
    mToken = mScanner.GetToken();
    
    MaybeBitExOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitOrExp" << endl;
} // Parser::MaybeBitOrExp()

void Parser::RestOfMaybeBitOrExp() {
  if ( DEBUG ) cout << "RestOfMaybeBitOrExp" << endl;
  RestOfMaybeBitExOrExp();

  while ( mToken.tokenType == BITOR ) {
    mToken = mScanner.GetToken();
    
    MaybeBitExOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitOrExp" << endl;
} // Parser::RestOfMaybeBitOrExp()

void Parser::MaybeBitExOrExp() {
  if ( DEBUG ) cout << "MaybeBitExOrExp" << endl;
  MaybeBitAndExp();

  while ( mToken.tokenType == BITXOR ) {
    mToken = mScanner.GetToken();
    
    MaybeBitAndExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitExOrExp" << endl;
} // Parser::MaybeBitExOrExp()

void Parser::RestOfMaybeBitExOrExp() {
  if ( DEBUG ) cout << "RestOfMaybeBitExOrExp" << endl;
  RestOfMaybeBitAndExp();

  while ( mToken.tokenType == BITXOR ) {
    mToken = mScanner.GetToken();
    
    MaybeBitAndExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitExOrExp" << endl;
} // Parser::RestOfMaybeBitExOrExp()

void Parser::MaybeBitAndExp() {
  if ( DEBUG ) cout << "MaybeBitAndExp" << endl;
  MaybeEqualityExp();
  
  while ( mToken.tokenType == BITAND ) {
    mToken = mScanner.GetToken();
    
    MaybeEqualityExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitAndExp" << endl;
} // Parser::MaybeBitAndExp()

void Parser::RestOfMaybeBitAndExp() {
  if ( DEBUG ) cout << "RestOfMaybeBitAndExp" << endl;
  RestOfMaybeEqualityExp();

  while ( mToken.tokenType == BITAND ) {
    mToken = mScanner.GetToken();
    
    MaybeEqualityExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitAndExp" << endl;
} // Parser::RestOfMaybeBitAndExp()

void Parser::MaybeEqualityExp() {
  if ( DEBUG ) cout << "MaybeEqualityExp" << endl;
  MaybeRelationalExp();

  while ( mToken.tokenType == NEQ || mToken.tokenType == EQ ) {
    mToken = mScanner.GetToken();
    
    MaybeRelationalExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeEqualityExp" << endl;
} // Parser::MaybeEqualityExp()

void Parser::RestOfMaybeEqualityExp() {
  if ( DEBUG ) cout << "RestOfMaybeEqualityExp" << endl;
  RestOfMaybeRelationalExp();

  while ( mToken.tokenType == NEQ || mToken.tokenType == EQ ) {
    mToken = mScanner.GetToken();
    
    MaybeRelationalExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeEqualityExp" << endl;
} // Parser::RestOfMaybeEqualityExp()

void Parser::MaybeRelationalExp() {
  if ( DEBUG ) cout << "MaybeRelationalExp" << endl;
  MaybeShiftExp();

  while ( mToken.tokenType == GE || mToken.tokenType == LE ||
          mToken.tokenType == LSS || mToken.tokenType == GTR ) {
    mToken = mScanner.GetToken();
    
    MaybeShiftExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeRelationalExp" << endl;
} // Parser::MaybeRelationalExp()

void Parser::RestOfMaybeRelationalExp() {
  if ( DEBUG ) cout << "RestOfMaybeRelationalExp" << endl;
  RestOfMaybeShiftExp();

  while ( mToken.tokenType == GE || mToken.tokenType == LE ||
          mToken.tokenType == LSS || mToken.tokenType == GTR ) {
    mToken = mScanner.GetToken();
    
    MaybeShiftExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeRelationalExp" << endl;
} // Parser::RestOfMaybeRelationalExp()

void Parser::MaybeShiftExp() {
  if ( DEBUG ) cout << "MaybeShiftExp" << endl;
  MaybeAdditiveExp();
  
  while ( mToken.tokenType == RS || mToken.tokenType == LS ||
          mToken.tokenType == LE || mToken.tokenType == GE ) {
    mToken = mScanner.GetToken();
    
    MaybeAdditiveExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeShiftExp" << endl;
} // Parser::MaybeShiftExp()

void Parser::RestOfMaybeShiftExp() {
  if ( DEBUG ) cout << "RestOfMaybeShiftExp" << endl;
  RestOfMaybeAdditiveExp();
  
  while ( mToken.tokenType == RS || mToken.tokenType == LS ||
          mToken.tokenType == LE || mToken.tokenType == GE ) {
    mToken = mScanner.GetToken();
    
    MaybeAdditiveExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeShiftExp" << endl;
} // Parser::RestOfMaybeShiftExp()

void Parser::MaybeAdditiveExp() {
  if ( DEBUG ) cout << "MaybeAdditiveExp" << endl;
  MaybeMultExp();
  
  while ( mToken.tokenType == ADD || mToken.tokenType == SUB ) {
    mToken = mScanner.GetToken();
    
    MaybeMultExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeAdditiveExp" << endl;
} // Parser::MaybeAdditiveExp()

void Parser::RestOfMaybeAdditiveExp() {
  if ( DEBUG ) cout << "RestOfMaybeAdditiveExp" << endl;

  RestOfMaybeMultExp();
  
  while ( mToken.tokenType == ADD || mToken.tokenType == SUB ) {
    mToken = mScanner.GetToken();
    
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

  while ( mToken.tokenType == MULT || mToken.tokenType == DIV || mToken.tokenType == REM ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    UnaryExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeMultExp" << endl;
} // Parser::RestOfMaybeMultExp()

void Parser::UnaryExp() {
  if ( DEBUG ) cout << mToken.tokenString << " UnaryExp" << endl;

  if ( mToken.tokenType == ADD || mToken.tokenType == SUB || mToken.tokenType == BITNOT ) {
    mToken = mScanner.GetToken();
  
    while ( mToken.tokenType == ADD || mToken.tokenType == SUB || mToken.tokenType == BITNOT ) {
      mToken = mScanner.GetToken();
    } // end while

    SignedUnaryExp();
  } // end if
  else if ( mToken.tokenType == ID || mToken.tokenType == LP || mToken.tokenType == CONSTANT ) {
    UnsignedUnaryExp();
  } // end if
  else if ( mToken.tokenType == PP || mToken.tokenType == MM ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == ID ) {
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == MLP ) {
        mToken = mScanner.GetToken();
        if ( mToken.tokenType == ID ||
             mToken.tokenType == PP ||
             mToken.tokenType == MM ||
             mToken.tokenType == ADD ||
             mToken.tokenType == SUB ||
             mToken.tokenType == BITNOT ||
             mToken.tokenType == CONSTANT ||
             mToken.tokenType == ID ||
             mToken.tokenType == LP ) {
          Expression();

          if ( mToken.tokenType != MRP ) {
            string errorMsg = "unexpected token : '" + mToken.tokenString + "'56";
            throw ( errorMsg );
          } // end if
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'57";
          throw ( errorMsg );
        } // end else
      } // end if
    } // end if
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'58";
    throw ( errorMsg );
  } // end else
  
  // mToken = mScanner.GetToken();
  if ( DEBUG ) cout << "end of UnaryExp" << endl;
} // Parser::UnaryExp()

void Parser::SignedUnaryExp() {
  if ( DEBUG ) cout << "SignedUnaryExp" << endl;

  if ( mToken.tokenType == ID ) {
    string name = mToken.tokenString;
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == LP ) {
      CheckFunctionNameExist( name );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mToken.tokenType == ID || mToken.tokenType == PP ||
           mToken.tokenType == MM || mToken.tokenType == ADD ||
           mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
        ActualParameterList();
      } // end if

      mToken = mScanner.GetToken();
      if ( mToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'59";
        throw ( errorMsg );
      } // end if
      else {
        mToken = mScanner.GetToken();
      } // end else
    } // end if
    else if ( mToken.tokenType == MLP ) {
      FindVariable( name, true );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == ID || mToken.tokenType == PP ||
           mToken.tokenType == MM || mToken.tokenType == ADD ||
           mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
      
        Expression();
        if ( mToken.tokenType != MRP ) {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'60";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'61";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      FindVariable( name, true );
    } // end else
  } // end if
  else if ( mToken.tokenType == LP ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == ID || mToken.tokenType == PP ||
         mToken.tokenType == MM || mToken.tokenType == ADD ||
         mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
      
      Expression();
      if ( mToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'62";
        throw ( errorMsg );
      } // end if
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'63";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mToken.tokenType == CONSTANT ) {
    mToken = mScanner.GetToken();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'64";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of SignedUnaryExp" << endl;
} // Parser::SignedUnaryExp()
  
void Parser::UnsignedUnaryExp() {
  if ( DEBUG ) cout << "UnsignedUnaryExp" << endl;

  if ( mToken.tokenType == ID ) {
    string name = mToken.tokenString;
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == LP ) {
      CheckFunctionNameExist( name );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if 

      if ( mToken.tokenType == ID || mToken.tokenType == PP ||
           mToken.tokenType == MM || mToken.tokenType == ADD ||
           mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
        ActualParameterList();
      } // end if
      else mToken = mScanner.GetToken();

      if ( mToken.tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'65";
        throw ( errorMsg );
      } // end if
      else {
        mToken = mScanner.GetToken();
      } // end else
    } // end if
    else if ( mToken.tokenType == MLP ) {
      FindVariable( name, true );
      mToken = mScanner.GetToken();
      if ( mToken.tokenType == ID || mToken.tokenType == PP ||
           mToken.tokenType == MM || mToken.tokenType == ADD ||
           mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
           mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
      
        Expression();
        if ( mToken.tokenType != MRP ) {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'66";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'67";
        throw ( errorMsg );
      } // end else

      mToken = mScanner.GetToken();
      if ( mToken.tokenType == PP || mToken.tokenType == MM ) {
        mToken = mScanner.GetToken(); // do something
      } // end if
    } // end if
    else if ( mToken.tokenType == PP || mToken.tokenType == MM ) {
      mToken = mScanner.GetToken(); // do something
    } // end if
    else {
      FindVariable( name, true );
    } // end else
  } // end if
  else if ( mToken.tokenType == LP ) {
    mToken = mScanner.GetToken();
    if ( mToken.tokenType == ID || mToken.tokenType == PP ||
         mToken.tokenType == MM || mToken.tokenType == ADD ||
         mToken.tokenType == SUB || mToken.tokenType == BITNOT ||
         mToken.tokenType == CONSTANT || mToken.tokenType == LP ) {
      
      Expression();
      if ( mToken.tokenType == RP ) {
        mToken = mScanner.GetToken();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'68";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'69";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mToken.tokenType == CONSTANT ) {
    mToken = mScanner.GetToken();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'64";
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
      if ( mVariableList[i].region >= mThreshold || mVariableList[i].region == 0 ) return i;
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
  int stack = -1;
  int preSpaceOneTime = 0;
  for ( int i = 0 ; i < mFunctionList.size() ; i++ ) {
    if ( name == mFunctionList[i].functionName ) {
      cout << mFunctionList[i].type << " " << mFunctionList[i].functionName;
      int next = 3;
      string nextToken;
      for ( int j = 2 ; j < mFunctionList[i].tokenList.size() ; j++ ) {
        cout << mFunctionList[i].tokenList[j].tokenString;
        if ( mFunctionList[i].tokenList[j].tokenString == "if" ) stack = 0;
        else if ( mFunctionList[i].tokenList[j].tokenString == "while" ) stack = 0;
        else if ( stack >= 0 && mFunctionList[i].tokenList[j].tokenString == "(" ) stack++;
        else if ( stack > 0 && mFunctionList[i].tokenList[j].tokenString == ")" ) {
          stack--;
          nextToken = mFunctionList[i].tokenList[next].tokenString;

          if ( stack == 0 && mFunctionList[i].tokenList[next].tokenString != "{" ) {
            stack = -1;
            preSpaceOneTime += 2;
            cout << endl;
            for ( int k = 0 ; k < preSpaceOneTime ; k++ ) {
              cout << " ";
            } // end for
          } // end if
        } // end if
        
        
        if ( next < mFunctionList[i].tokenList.size() ) 
          nextToken = mFunctionList[i].tokenList[next].tokenString;

        if ( j == mFunctionList[i].tokenList.size() - 2 ) cout << endl;
        else if ( j == mFunctionList[i].tokenList.size() - 1 ) ;
        else if ( mFunctionList[i].tokenList[j].tokenString == ";" ) {
          cout << endl;
          preSpaceOneTime = 0;
          if ( next < mFunctionList[i].tokenList.size() && 
               nextToken == "}" ) preSpace = preSpace - 2;

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
                  mFunctionList[i].tokenList[j + 1].tokenString != "++" &&
                  mFunctionList[i].tokenList[j + 1].tokenString != "--" &&
                  mFunctionList[i].tokenList[j + 1].tokenString != "," ) cout << " ";

        next++;
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

void Parser::CompoundStatementOp( string &result ) {
  mRegion++;
  int findIf = -1;

  if ( DEBUG ) cout << "CompoundStatement" << endl;
  mCur++;
  if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( IsDefOrStatement() ) {
    while ( IsDefOrStatement() ) {
           
      if ( mScanner.mTokenList[mCur].tokenType == INT || 
           mScanner.mTokenList[mCur].tokenType == CHAR || 
           mScanner.mTokenList[mCur].tokenType == FLOAT || 
           mScanner.mTokenList[mCur].tokenType == STRING || 
           mScanner.mTokenList[mCur].tokenType == BOOL ) {
        Declaration( true );
      } // end if
      else if ( mScanner.mTokenList[mCur].tokenType == ID ||
                mScanner.mTokenList[mCur].tokenType == PP ||
                mScanner.mTokenList[mCur].tokenType == MM ||
                mScanner.mTokenList[mCur].tokenType == ADD ||
                mScanner.mTokenList[mCur].tokenType == SUB ||
                mScanner.mTokenList[mCur].tokenType == BITNOT ||
                mScanner.mTokenList[mCur].tokenType == CONSTANT ||
                mScanner.mTokenList[mCur].tokenType == RETURN ||
                mScanner.mTokenList[mCur].tokenType == BLP ||
                mScanner.mTokenList[mCur].tokenType == IF || 
                mScanner.mTokenList[mCur].tokenType == WHILE ||
                mScanner.mTokenList[mCur].tokenType == DO ||
                mScanner.mTokenList[mCur].tokenType == SCLN ||
                mScanner.mTokenList[mCur].tokenType == LP ||
                mScanner.mTokenList[mCur].tokenType == CIN ||
                mScanner.mTokenList[mCur].tokenType == COUT ) {

        Statement();
      } // end if
      
      if ( ! mJustPeek ) mCur++;
      else {
        mToken = mScanner.mToken;
        mJustPeek = false;
      } // end else

      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
    } // end while
  } // end if
  
  if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType != BRP ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'23";
    throw ( errorMsg );
  } // end if
  else {
    mRegion--;
    RemoveVariables();
  } // end else
  
  if ( DEBUG ) cout << "end of CompoundStatement" << endl;
  mLastStatementIf = false;
} // Parser::CompoundStatementOp()

void Parser::StatementOp( string &result ) {
  if ( DEBUG ) cout << "Statement" << endl;
  mLastStatementIf = false;
  
  if ( DEBUG ) cout << mToken.tokenString << mIfStatement << endl;
  
  if ( mScanner.mTokenList[mCur].tokenType == SCLN ) return; // condition 1
  else if ( mScanner.mTokenList[mCur].tokenType == ID ||
            mScanner.mTokenList[mCur].tokenType == PP ||
            mScanner.mTokenList[mCur].tokenType == MM ||
            mScanner.mTokenList[mCur].tokenType == ADD ||
            mScanner.mTokenList[mCur].tokenType == SUB ||
            mScanner.mTokenList[mCur].tokenType == BITNOT ||
            mScanner.mTokenList[mCur].tokenType == CONSTANT ||
            mScanner.mTokenList[mCur].tokenType == LP ) { // condition 2

    Expression();
    
    if ( mScanner.mTokenList[mCur].tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'26";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == RETURN ) { // condition 3
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM ||
         mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB ||
         mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT ||
         mScanner.mTokenList[mCur].tokenType == LP ) {
      
      Expression();
    } // end if
    
    if ( mScanner.mTokenList[mCur].tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'27";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == BLP ) { // condition 4
    CompoundStatement();
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == IF ) { // condition 5
    mLastStatementIf = StatementIf();
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == WHILE ) { // condition 6
    StatementWhile();
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == DO ) { // condition 7
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    Statement();

    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType == WHILE ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mScanner.mTokenList[mCur].tokenType == LP ) {
        mCur++;
        if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if

        if ( mScanner.mTokenList[mCur].tokenType == ID ||
             mScanner.mTokenList[mCur].tokenType == PP ||
             mScanner.mTokenList[mCur].tokenType == MM ||
             mScanner.mTokenList[mCur].tokenType == ADD ||
             mScanner.mTokenList[mCur].tokenType == SUB ||
             mScanner.mTokenList[mCur].tokenType == BITNOT ||
             mScanner.mTokenList[mCur].tokenType == CONSTANT ||
             mScanner.mTokenList[mCur].tokenType == ID ||
             mScanner.mTokenList[mCur].tokenType == LP ) {
          Expression();
        } // end if
        else if ( mScanner.mTokenList[mCur].tokenType == COUT ) {
          CoutExpression();
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'34";
          throw ( errorMsg );
        } // end else
    
        if ( mScanner.mTokenList[mCur].tokenType != RP ) {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'35";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'29";
        throw ( errorMsg );
      } // end else
      
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mScanner.mTokenList[mCur].tokenType != SCLN ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'30";
        throw ( errorMsg );
      } // end if
    } // end if
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == ELSE && mIfStatement ) {
    mIfStatement--;
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    Statement();
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == COUT ) {
    CoutExpression();
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'30";
      throw ( errorMsg );
    } // end if
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == CIN ) {
    CinExpression();
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'3'";
    throw ( errorMsg );
  } // end else
  
  if ( DEBUG ) cout << "end of Statement" << endl;
} // Parser::Statement()

void Parser::CoutExpressionOp( string &result ) {
  if ( DEBUG ) cout << "CoutExpression" << endl;
  mCur++;
  if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if

  if ( mScanner.mTokenList[mCur].tokenType != LS ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
    throw ( errorMsg );
  } // end if

  while ( mScanner.mTokenList[mCur].tokenType == LS ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM ||
         mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB ||
         mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT ||
         mScanner.mTokenList[mCur].tokenType == LP ) { // condition 2

      Expression();

    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
      throw ( errorMsg );
    } // end else
  } // end while
    
} // Parser::CoutExpression()

void Parser::CinExpressionOp( string &result ) {
  if ( DEBUG ) cout << "CinExpression" << endl;
  mCur++;
  if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if

  if ( mScanner.mTokenList[mCur].tokenType == RS ) {
    while ( mScanner.mTokenList[mCur].tokenType == RS ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == LP || mScanner.mTokenList[mCur].tokenType == CONSTANT ) {
        UnsignedUnaryExp() ;
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
        throw ( errorMsg );
      } // end else
    } // end while

    if ( mScanner.mTokenList[mCur].tokenType != SCLN ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'35";
      throw ( errorMsg );
    } // end if
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'36";
    throw ( errorMsg );
  } // end else
} // Parser::CinExpression()

bool Parser::StatementIfOp( string &result ) {
  if ( DEBUG ) cout << " StatementIf" << endl;
  mCur++;
  mIfStatement++;

  if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
    
  if ( mScanner.mTokenList[mCur].tokenType == LP ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM ||
         mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB ||
         mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT || 
         mScanner.mTokenList[mCur].tokenType == LP ) {
      Expression();
    } // end if
    else if ( mScanner.mTokenList[mCur].tokenType == COUT ) {
      CoutExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'31";
      throw ( errorMsg );
    } // end else
    
    if ( mScanner.mTokenList[mCur].tokenType != RP ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'32";
      throw ( errorMsg );
    } // end if
    
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM ||
         mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB ||
         mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT ||
         mScanner.mTokenList[mCur].tokenType == RETURN ||
         mScanner.mTokenList[mCur].tokenType == BLP ||
         mScanner.mTokenList[mCur].tokenType == IF || 
         mScanner.mTokenList[mCur].tokenType == WHILE ||
         mScanner.mTokenList[mCur].tokenType == DO ||
         mScanner.mTokenList[mCur].tokenType == SCLN ||
         mScanner.mTokenList[mCur].tokenType == CIN ||
         mScanner.mTokenList[mCur].tokenType == COUT ||
         mScanner.mTokenList[mCur].tokenType == LP ) {
      Statement();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'33";
      throw ( errorMsg );
    } // end else
    
    
    mScanner.GetToken();

    if ( mScanner.mScanner.mTokenList[mCur].tokenType == ELSE ) {
      mToken = mScanner.mToken;
      Statement();
      return false;
    } // end if
    else {
      
      mJustPeek = true;
    } // end else
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'31";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << " end of StatementIf" << endl;

  return true;
} // Parser::StatementIf()

void Parser::StatementWhileOp( string &result ) {
  if ( DEBUG ) cout << "StatementWhile" << endl;
  mCur++;
  if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
    
  if ( mScanner.mTokenList[mCur].tokenType == LP ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM ||
         mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB ||
         mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT ||
         mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == LP ) {
      Expression();
    } // end if
    else if ( mScanner.mTokenList[mCur].tokenType == COUT ) {
      CoutExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'34";
      throw ( errorMsg );
    } // end else
    
    if ( mScanner.mTokenList[mCur].tokenType != RP ) {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'35";
      throw ( errorMsg );
    } // end if
    
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM ||
         mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB ||
         mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT ||
         mScanner.mTokenList[mCur].tokenType == RETURN ||
         mScanner.mTokenList[mCur].tokenType == BLP ||
         mScanner.mTokenList[mCur].tokenType == IF || 
         mScanner.mTokenList[mCur].tokenType == WHILE ||
         mScanner.mTokenList[mCur].tokenType == DO ||
         mScanner.mTokenList[mCur].tokenType == SCLN ||
         mScanner.mTokenList[mCur].tokenType == CIN ||
         mScanner.mTokenList[mCur].tokenType == COUT ||
         mScanner.mTokenList[mCur].tokenType == LP ) {
      Statement();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'36";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'31";
    throw ( errorMsg );
  } // end else
  
  if ( DEBUG ) cout << "end of StatementWhile" << endl;
} // Parser::StatementWhile()

bool Parser::IsDefOrStatementOp( string &result ) {
  if ( DEBUG ) cout << "IsDefOrStatement" << endl;
  return ( mScanner.mTokenList[mCur].tokenType == INT || 
           mScanner.mTokenList[mCur].tokenType == CHAR || 
           mScanner.mTokenList[mCur].tokenType == FLOAT || 
           mScanner.mTokenList[mCur].tokenType == STRING || 
           mScanner.mTokenList[mCur].tokenType == BOOL ||
           mScanner.mTokenList[mCur].tokenType == ID ||
           mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM ||
           mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB ||
           mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT ||
           mScanner.mTokenList[mCur].tokenType == RETURN ||
           mScanner.mTokenList[mCur].tokenType == BLP ||
           mScanner.mTokenList[mCur].tokenType == IF || 
           mScanner.mTokenList[mCur].tokenType == WHILE ||
           mScanner.mTokenList[mCur].tokenType == DO ||
           mScanner.mTokenList[mCur].tokenType == SCLN ||
           mScanner.mTokenList[mCur].tokenType == LP ||
           mScanner.mTokenList[mCur].tokenType == CIN ||
           mScanner.mTokenList[mCur].tokenType == COUT );
} // Parser::IsDefOrStatement()

void Parser::ExpressionOp( string &result ) {
  if ( DEBUG ) cout << "Expression" << endl;
  BasicExpression();
  
  if ( mScanner.mTokenList[mCur].tokenType == COMMA ) {
    while ( mScanner.mTokenList[mCur].tokenType == COMMA ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mScanner.mTokenList[mCur].tokenType == ID ||
           mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM ||
           mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB ||
           mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT ||
           mScanner.mTokenList[mCur].tokenType == LP ) {
        BasicExpression();
      } // end if
      
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    } // end while
  } // end if
  
  if ( DEBUG ) cout << "end of Expression" << endl;
} // Parser::Expression()

void Parser::BasicExpressionOp( string &result ) {
  if ( DEBUG ) cout << "BasicExpression" << endl;
  if ( mScanner.mTokenList[mCur].tokenType == ID ) {
    // FindVariable( mToken.tokenString, true );
    string name = mToken.tokenString;
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) { 
      string error = "EOF";
      throw ( error );
    } // end if
    
    RestOfIDStartedBasicExp( name );
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == PP || mScanner.mTokenList[mCur].tokenType == MM ) { // condition 2
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mScanner.mTokenList[mCur].tokenType == ID ) {
      FindVariable( mToken.tokenString, true );
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      RestOfPPMMIDStartedBasicExp();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'38";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == ADD || mScanner.mTokenList[mCur].tokenType == SUB || 
            mScanner.mTokenList[mCur].tokenType == BITNOT ) { // condition 3
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    while ( mScanner.mTokenList[mCur].tokenType == ADD || mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    } // end while
    
    if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == CONSTANT || 
         mScanner.mTokenList[mCur].tokenType == LP ) {
      SignedUnaryExp();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'39";
      throw ( errorMsg );
    } // end else
  
    RomceAndRomloe();
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) { // condition 4
    if ( mScanner.mTokenList[mCur].tokenType == LP ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    
      if ( mScanner.mTokenList[mCur].tokenType == ID ||
           mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM ||
           mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB ||
           mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT ||
           mScanner.mTokenList[mCur].tokenType == ID ||
           mScanner.mTokenList[mCur].tokenType == LP ) {
        Expression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'40";
        throw ( errorMsg );
      } // end else
      
      if ( mScanner.mTokenList[mCur].tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'41";
        throw ( errorMsg );      
      } // end if
    } // end if
  
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if	

    RomceAndRomloe();
  } // end if
  
  if ( DEBUG ) cout << "end of BasicExpression" << endl;
} // Parser::BasicExpression()

void Parser::RestOfIDStartedBasicExpOp( string name, string &result ) {
  if ( DEBUG ) cout << "RestOfIDStartedBasicExp" << endl;
  if ( mScanner.mTokenList[mCur].tokenType == MLP || mScanner.mTokenList[mCur].tokenType == ASSIGN || 
       mScanner.mTokenList[mCur].tokenType == TE || mScanner.mTokenList[mCur].tokenType == DE || 
       mScanner.mTokenList[mCur].tokenType == RE || mScanner.mTokenList[mCur].tokenType == PE || 
       mScanner.mTokenList[mCur].tokenType == ME || mScanner.mTokenList[mCur].tokenType == PP ||
       mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD || 
       mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
       mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == CONSTANT ) { // condition 1

    if ( mScanner.mTokenList[mCur].tokenType == MLP ) {
      FindVariable( name, true );
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
    
      if ( mScanner.mTokenList[mCur].tokenType == ID ||
           mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM ||
           mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB ||
           mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT ||
           mScanner.mTokenList[mCur].tokenType == ID ||
           mScanner.mTokenList[mCur].tokenType == LP ) {
        Expression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'42";
        throw ( errorMsg );
      } // end else
    
      if ( mScanner.mTokenList[mCur].tokenType != MRP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'43";
        throw ( errorMsg );
      } // end if
      else {
        mCur++;
      } // end else
    } // end if
    
    if ( mScanner.mTokenList[mCur].tokenType == ASSIGN || mScanner.mTokenList[mCur].tokenType == TE || 
         mScanner.mTokenList[mCur].tokenType == DE || mScanner.mTokenList[mCur].tokenType == RE || 
         mScanner.mTokenList[mCur].tokenType == PE || mScanner.mTokenList[mCur].tokenType == ME ) {

      FindVariable( name, true );
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if
      
      if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
        BasicExpression();
      } // end if
    } // end if
    else  {
      if ( mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == PP ) mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      FindVariable( name, true );
      RomceAndRomloe();
    } // end else
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == LP ) {
    CheckFunctionNameExist( name );
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
      ActualParameterList();
      
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType == RP ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      RomceAndRomloe();
      /*
      else  {
        string errorMsg = "unexpected token '" + mToken.tokenString + "'\n45";
        throw ( errorMsg );
      } // end else
      */
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'46";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    FindVariable( name, true );
    RomceAndRomloe();
  } // end else
  
  if ( DEBUG ) cout << "end of RestOfIDStartedBasicExp" << endl;
} // Parser::RestOfIDStartedBasicExp()

void Parser::RestOfPPMMIDStartedBasicExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfPPMMIDStartedBasicExp" << endl;
  if ( mScanner.mTokenList[mCur].tokenType == MLP ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
    
    if ( mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM ||
         mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB ||
         mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT ||
         mScanner.mTokenList[mCur].tokenType == ID ||
         mScanner.mTokenList[mCur].tokenType == LP ) {
      Expression();
      
      
      if ( mScanner.mTokenList[mCur].tokenType == MRP ) {
        mCur++;
        if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
          string error = "EOF";
          throw ( error );
        } // end if
        
        if ( mScanner.mTokenList[mCur].tokenType == PP || mScanner.mTokenList[mCur].tokenType == LP ||
             mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD || 
             mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
             mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == CONSTANT ) {

          RomceAndRomloe();
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'47";
          throw ( errorMsg );
        } // end else
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'48";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'49";
      throw ( errorMsg );
    } // end else
  } // end if
  else {
    RomceAndRomloe();
  } // end else
  
  if ( DEBUG ) cout << "end of RestOfPPMMIDStartedBasicExp" << endl;
} // Parser::RestOfPPMMIDStartedBasicExp() ;

void Parser::SignOp( string &result ) {
  if ( DEBUG ) cout << "Sign" << endl;
  if ( mScanner.mTokenList[mCur].tokenType != ADD && mScanner.mTokenList[mCur].tokenType != SUB && mScanner.mTokenList[mCur].tokenType != BITNOT ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'50";
    throw ( errorMsg );
  } // end if
} // Parser::Sign()

void Parser::ActualParameterListOp( string &result ) {
  if ( DEBUG ) cout << "ActualParameterList" << endl;
  BasicExpression();
  
  while ( mScanner.mTokenList[mCur].tokenType == COMMA ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
      BasicExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'51";
      throw ( errorMsg );
    } // end else
    

  } // end while
  
  if ( DEBUG ) cout << "end of ActualParameterList" << endl;
} // Parser::ActualParameterList()

void Parser::AssignmentOperatorOp( string &result ) {
  if ( DEBUG ) cout << "AssignmentOperator" << endl;
  if ( mScanner.mTokenList[mCur].tokenType != ASSIGN && mScanner.mTokenList[mCur].tokenType != TE && 
       mScanner.mTokenList[mCur].tokenType != DE && mScanner.mTokenList[mCur].tokenType != RE &&
       mScanner.mTokenList[mCur].tokenType != PE && mScanner.mTokenList[mCur].tokenType != ME ) {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'52";
    throw ( errorMsg );
  } // end if
} // Parser::AssignmentOperator()

void Parser::RomceAndRomloeOp( string &result ) {
  if ( DEBUG ) cout << "RomceAndRomloe" << endl;
  
  RestOfMaybeLogicalOrExp();
  
  // mCur++;
  if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
    string error = "EOF";
    throw ( error );
  } // end if
  
  if ( mScanner.mTokenList[mCur].tokenType == QUE ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if
  
    if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
      BasicExpression();
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'53";
      throw ( errorMsg );
    } // end else
    
    if ( mScanner.mTokenList[mCur].tokenType == COLON ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
        BasicExpression();
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'54";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'55";
      throw ( errorMsg );
    } // end else
  } // end if
  
  if ( DEBUG ) cout << "end of RomceAndRomloe" << endl;
} // Parser::RomceAndRomloe()

void Parser::RestOfMaybeLogicalOrExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeLogicalOrExp" << endl;
  RestOfMaybeLogicalAndExp();
  
  
  while ( mScanner.mTokenList[mCur].tokenType == OR ) {
    mCur++;
    
    MaybeLogicalAndExp();
  } // end while

  if ( DEBUG ) cout << "end of RestOfMaybeLogicalOrExp" << endl;
} // Parser::RestOfMaybeLogicalOrExp()

void Parser::MaybeLogicalAndExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeLogicalAndExp" << endl;
  MaybeBitOrExp();
  
  
  while ( mScanner.mTokenList[mCur].tokenType == AND ) {
    mCur++;
    
    MaybeBitOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeLogicalAndExp" << endl;
} // Parser::MaybeLogicalAndExp()

void Parser::RestOfMaybeLogicalAndExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeLogicalAndExp" << endl;
  RestOfMaybeBitOrExp();
  
  while ( mScanner.mTokenList[mCur].tokenType == AND ) {
    mCur++;
    
    MaybeBitOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeLogicalAndExp" << endl;
} // Parser::RestOfMaybeLogicalAndExp()

void Parser::MaybeBitOrExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeBitOrExp" << endl;
  MaybeBitExOrExp();
  
  while ( mScanner.mTokenList[mCur].tokenType == BITOR ) {
    mCur++;
    
    MaybeBitExOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitOrExp" << endl;
} // Parser::MaybeBitOrExp()

void Parser::RestOfMaybeBitOrExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeBitOrExp" << endl;
  RestOfMaybeBitExOrExp();

  while ( mScanner.mTokenList[mCur].tokenType == BITOR ) {
    mCur++;
    
    MaybeBitExOrExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitOrExp" << endl;
} // Parser::RestOfMaybeBitOrExp()

void Parser::MaybeBitExOrExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeBitExOrExp" << endl;
  MaybeBitAndExp();

  while ( mScanner.mTokenList[mCur].tokenType == BITXOR ) {
    mCur++;
    
    MaybeBitAndExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitExOrExp" << endl;
} // Parser::MaybeBitExOrExp()

void Parser::RestOfMaybeBitExOrExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeBitExOrExp" << endl;
  RestOfMaybeBitAndExp();

  while ( mScanner.mTokenList[mCur].tokenType == BITXOR ) {
    mCur++;
    
    MaybeBitAndExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitExOrExp" << endl;
} // Parser::RestOfMaybeBitExOrExp()

void Parser::MaybeBitAndExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeBitAndExp" << endl;
  MaybeEqualityExp();
  
  while ( mScanner.mTokenList[mCur].tokenType == BITAND ) {
    mCur++;
    
    MaybeEqualityExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeBitAndExp" << endl;
} // Parser::MaybeBitAndExp()

void Parser::RestOfMaybeBitAndExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeBitAndExp" << endl;
  RestOfMaybeEqualityExp();

  while ( mScanner.mTokenList[mCur].tokenType == BITAND ) {
    mCur++;
    
    MaybeEqualityExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeBitAndExp" << endl;
} // Parser::RestOfMaybeBitAndExp()

void Parser::MaybeEqualityExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeEqualityExp" << endl;
  MaybeRelationalExp();

  while ( mScanner.mTokenList[mCur].tokenType == NEQ || mScanner.mTokenList[mCur].tokenType == EQ ) {
    mCur++;
    
    MaybeRelationalExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeEqualityExp" << endl;
} // Parser::MaybeEqualityExp()

void Parser::RestOfMaybeEqualityExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeEqualityExp" << endl;
  RestOfMaybeRelationalExp();

  while ( mScanner.mTokenList[mCur].tokenType == NEQ || mScanner.mTokenList[mCur].tokenType == EQ ) {
    mCur++;
    
    MaybeRelationalExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeEqualityExp" << endl;
} // Parser::RestOfMaybeEqualityExp()

void Parser::MaybeRelationalExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeRelationalExp" << endl;
  MaybeShiftExp();

  while ( mScanner.mTokenList[mCur].tokenType == GE || mScanner.mTokenList[mCur].tokenType == LE ||
          mScanner.mTokenList[mCur].tokenType == LSS || mScanner.mTokenList[mCur].tokenType == GTR ) {
    mCur++;
    
    MaybeShiftExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeRelationalExp" << endl;
} // Parser::MaybeRelationalExp()

void Parser::RestOfMaybeRelationalExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeRelationalExp" << endl;
  RestOfMaybeShiftExp();

  while ( mScanner.mTokenList[mCur].tokenType == GE || mScanner.mTokenList[mCur].tokenType == LE ||
          mScanner.mTokenList[mCur].tokenType == LSS || mScanner.mTokenList[mCur].tokenType == GTR ) {
    mCur++;
    
    MaybeShiftExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeRelationalExp" << endl;
} // Parser::RestOfMaybeRelationalExp()

void Parser::MaybeShiftExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeShiftExp" << endl;
  MaybeAdditiveExp();
  
  while ( mScanner.mTokenList[mCur].tokenType == RS || mScanner.mTokenList[mCur].tokenType == LS ||
          mScanner.mTokenList[mCur].tokenType == LE || mScanner.mTokenList[mCur].tokenType == GE ) {
    mCur++;
    
    MaybeAdditiveExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeShiftExp" << endl;
} // Parser::MaybeShiftExp()

void Parser::RestOfMaybeShiftExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeShiftExp" << endl;
  RestOfMaybeAdditiveExp();
  
  while ( mScanner.mTokenList[mCur].tokenType == RS || mScanner.mTokenList[mCur].tokenType == LS ||
          mScanner.mTokenList[mCur].tokenType == LE || mScanner.mTokenList[mCur].tokenType == GE ) {
    mCur++;
    
    MaybeAdditiveExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeShiftExp" << endl;
} // Parser::RestOfMaybeShiftExp()

void Parser::MaybeAdditiveExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeAdditiveExp" << endl;
  MaybeMultExp();
  
  while ( mScanner.mTokenList[mCur].tokenType == ADD || mScanner.mTokenList[mCur].tokenType == SUB ) {
    mCur++;
    
    MaybeMultExp();
  } // end while
  
  if ( DEBUG ) cout << "end of MaybeAdditiveExp" << endl;
} // Parser::MaybeAdditiveExp()

void Parser::RestOfMaybeAdditiveExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeAdditiveExp" << endl;

  RestOfMaybeMultExp();
  
  while ( mScanner.mTokenList[mCur].tokenType == ADD || mScanner.mTokenList[mCur].tokenType == SUB ) {
    mCur++;
    
    MaybeMultExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeAdditiveExp" << endl;
} // Parser::RestOfMaybeAdditiveExp()

void Parser::MaybeMultExpOp( string &result ) {
  if ( DEBUG ) cout << "MaybeMultExp" << endl;

  UnaryExp();

  RestOfMaybeMultExp();
  
  if ( DEBUG ) cout << "end of MaybeMultExp" << endl;
} // Parser::MaybeMultExp()

void Parser::RestOfMaybeMultExpOp( string &result ) {
  if ( DEBUG ) cout << "RestOfMaybeMultExp" << endl;

  while ( mScanner.mTokenList[mCur].tokenType == MULT || mScanner.mTokenList[mCur].tokenType == DIV || mScanner.mTokenList[mCur].tokenType == REM ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
      string error = "EOF";
      throw ( error );
    } // end if

    UnaryExp();
  } // end while
  
  if ( DEBUG ) cout << "end of RestOfMaybeMultExp" << endl;
} // Parser::RestOfMaybeMultExp()

void Parser::UnaryExpOp( string &result ) {
  if ( DEBUG ) cout << mToken.tokenString << " UnaryExp" << endl;

  if ( mScanner.mTokenList[mCur].tokenType == ADD || mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ) {
    mCur++;
  
    while ( mScanner.mTokenList[mCur].tokenType == ADD || mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ) {
      mCur++;
    } // end while

    SignedUnaryExp();
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == LP || mScanner.mTokenList[mCur].tokenType == CONSTANT ) {
    UnsignedUnaryExp();
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == PP || mScanner.mTokenList[mCur].tokenType == MM ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == ID ) {
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == MLP ) {
        mCur++;
        if ( mScanner.mTokenList[mCur].tokenType == ID ||
             mScanner.mTokenList[mCur].tokenType == PP ||
             mScanner.mTokenList[mCur].tokenType == MM ||
             mScanner.mTokenList[mCur].tokenType == ADD ||
             mScanner.mTokenList[mCur].tokenType == SUB ||
             mScanner.mTokenList[mCur].tokenType == BITNOT ||
             mScanner.mTokenList[mCur].tokenType == CONSTANT ||
             mScanner.mTokenList[mCur].tokenType == ID ||
             mScanner.mTokenList[mCur].tokenType == LP ) {
          Expression();

          if ( mScanner.mTokenList[mCur].tokenType != MRP ) {
            string errorMsg = "unexpected token : '" + mToken.tokenString + "'56";
            throw ( errorMsg );
          } // end if
        } // end if
        else {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'57";
          throw ( errorMsg );
        } // end else
      } // end if
    } // end if
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'58";
    throw ( errorMsg );
  } // end else
  
  // mCur++;
  if ( DEBUG ) cout << "end of UnaryExp" << endl;
} // Parser::UnaryExp()

void Parser::SignedUnaryExpOp( string &result ) {
  if ( DEBUG ) cout << "SignedUnaryExp" << endl;

  if ( mScanner.mTokenList[mCur].tokenType == ID ) {
    string name = mToken.tokenString;
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == LP ) {
      CheckFunctionNameExist( name );
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if

      if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
        ActualParameterList();
      } // end if

      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'59";
        throw ( errorMsg );
      } // end if
      else {
        mCur++;
      } // end else
    } // end if
    else if ( mScanner.mTokenList[mCur].tokenType == MLP ) {
      FindVariable( name, true );
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
      
        Expression();
        if ( mScanner.mTokenList[mCur].tokenType != MRP ) {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'60";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'61";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      FindVariable( name, true );
    } // end else
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == LP ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
      
      Expression();
      if ( mScanner.mTokenList[mCur].tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'62";
        throw ( errorMsg );
      } // end if
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'63";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == CONSTANT ) {
    mCur++;
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'64";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of SignedUnaryExp" << endl;
} // Parser::SignedUnaryExp()
  
void Parser::UnsignedUnaryExpOp( string &result ) {
  if ( DEBUG ) cout << "UnsignedUnaryExp" << endl;

  if ( mScanner.mTokenList[mCur].tokenType == ID ) {
    string name = mToken.tokenString;
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == LP ) {
      CheckFunctionNameExist( name );
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == END_OF_FILE ) {
        string error = "EOF";
        throw ( error );
      } // end if 

      if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
        ActualParameterList();
      } // end if
      else mCur++;

      if ( mScanner.mTokenList[mCur].tokenType != RP ) {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'65";
        throw ( errorMsg );
      } // end if
      else {
        mCur++;
      } // end else
    } // end if
    else if ( mScanner.mTokenList[mCur].tokenType == MLP ) {
      FindVariable( name, true );
      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
           mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
           mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
           mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
      
        Expression();
        if ( mScanner.mTokenList[mCur].tokenType != MRP ) {
          string errorMsg = "unexpected token : '" + mToken.tokenString + "'66";
          throw ( errorMsg );
        } // end if
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'67";
        throw ( errorMsg );
      } // end else

      mCur++;
      if ( mScanner.mTokenList[mCur].tokenType == PP || mScanner.mTokenList[mCur].tokenType == MM ) {
        mCur++; // do something
      } // end if
    } // end if
    else if ( mScanner.mTokenList[mCur].tokenType == PP || mScanner.mTokenList[mCur].tokenType == MM ) {
      mCur++; // do something
    } // end if
    else {
      FindVariable( name, true );
    } // end else
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == LP ) {
    mCur++;
    if ( mScanner.mTokenList[mCur].tokenType == ID || mScanner.mTokenList[mCur].tokenType == PP ||
         mScanner.mTokenList[mCur].tokenType == MM || mScanner.mTokenList[mCur].tokenType == ADD ||
         mScanner.mTokenList[mCur].tokenType == SUB || mScanner.mTokenList[mCur].tokenType == BITNOT ||
         mScanner.mTokenList[mCur].tokenType == CONSTANT || mScanner.mTokenList[mCur].tokenType == LP ) {
      
      Expression();
      if ( mScanner.mTokenList[mCur].tokenType == RP ) {
        mCur++;
      } // end if
      else {
        string errorMsg = "unexpected token : '" + mToken.tokenString + "'68";
        throw ( errorMsg );
      } // end else
    } // end if
    else {
      string errorMsg = "unexpected token : '" + mToken.tokenString + "'69";
      throw ( errorMsg );
    } // end else
  } // end if
  else if ( mScanner.mTokenList[mCur].tokenType == CONSTANT ) {
    mCur++;
  } // end if
  else {
    string errorMsg = "unexpected token : '" + mToken.tokenString + "'64";
    throw ( errorMsg );
  } // end else

  if ( DEBUG ) cout << "end of UnsignedUnaryExp" << endl;
} // Parser::UnsignedUnaryExp()

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
    ;
  } // while
  
  printf( "Our-C exited ..." ) ;
} // main()
