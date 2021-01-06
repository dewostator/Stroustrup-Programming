/*”пражение 7.6
добавить справку по нажатию клавиши H */
/*
	calculator08buggy.cpp

	Helpful comments removed.

	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/
/* GRAMMAR
 * =======
 * Calculation:
 *      Statement
 *      Print
 *      Quit
 *      Calculation Statement
 *
 * Statement:
 *      Declaration
 *      Expression
 * 
 * Declaration:
 *      "let" Name "=" Expression
 *
 * Expression:
 *      Term
 *      Expression + Term
 *      Expression - Term
 *
 * Term:
 *      Secondary
 *      Term * Secondary
 *      Term / Secondary
 *      Term % Secondary
 *
 * Secondary:
 *      Primary
 *      Secondary ^ Primary
 *
 * Primary:
 *      Number
 *		Name.value
 *      ( Expression )
 *      - Primary
 *      ? Primary
 *      Primary ^ Primary
 *
 * Number:
 *      floating-point-literal

 */



#include "../../std_lib_facilities.h"


const char let = '#';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char square_root = '?';
const char exponent = '^';
const char Const = 'C';


void print_help()
{
	cout << "help message\n" << "H - for help\n" << "q - for quit\n";

}


struct Token {
    // All Tokens have a 'kind', value and name are optional
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
    Token(char ch, string s) :kind(ch), name(s) { }
};


struct Variable {
    // Every variable has a name and a value
	string name;
	double value;
	bool isConst;
	Variable(string n, double v) :name(n), value(v), isConst(false) { }
	Variable(string n, double v, bool isConst) :name(n), value(v), isConst(true) { }
};



class Symbol_table
{
	public:
		vector<Variable> var_table;
		
		double get(string s);
		void set(string s, double d);
		bool is_declared(string s);
		double define_name(string var, double val);

};

Symbol_table st;

class Token_stream {
	bool full;
	Token buffer;
public:
    // Constructor
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer=t; full=true; }

	void ignore(char);
};



Token Token_stream::get()
{
    // Check buffer first
	if (full) { full=false; return buffer; }

    // Otherwise process next Token
	char ch='x';
	do
	{
		cin.get(ch);
		if (ch == '\n')
		{
			ch = print;
			//break;
		}
	}
	while(isspace(ch));

	switch (ch) {
    case quit:
    case square_root:
    case exponent:
    case let:
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	
        // if digit, back up & take whole number
        cin.putback(ch);
		double val;
		cin >> val;
		return Token(number,val);
	}
	default:
		if (isalpha(ch)) {
            // same thing if Token is unregistered, we want to accumulate
            // alpha chars and digits into variable name
			string s;
			s += ch;
			while(cin.get(ch) && (isalpha(ch) || isdigit(ch) || (ch == '_') )) s += ch;
			cin.putback(ch);
			if (s == "const") return Token(Const);
			if (s == "let") return Token(let);
			if (s == "exit") return Token(quit);
			if (s == "H")
			{
				print_help();
				return Token(quit);
			}
			return Token(name,s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
    // ignore all tokens up to first instance of 'c'
{
	if (full && c==buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin>>ch)
		if (ch==c) return;
}

	

double Symbol_table::get(string s)
    // retrieves a variable value from variable vector
{
	for (int i = 0; i<var_table.size(); ++i)
		if (var_table[i].name == s) return var_table[i].value;
	error("get: undefined name ",s);
}

void Symbol_table::set(string s, double d)
    // changes the value of an existing variable
{
	for (int i = 0; i<var_table.size(); ++i)
	{
		if (var_table[i].name == s)
		{
			if (var_table[i].isConst == true)
				error("set: can't change const ",s);
			var_table[i].value = d;
			return;
		}
	}
	error("set: undefined name ",s);
}

bool Symbol_table::is_declared(string s)
    // checks to see if a given variable name has already been declared
{
	for (int i = 0; i<var_table.size(); ++i)
		if (var_table[i].name == s) return true;
	return false;
}

double Symbol_table::define_name(string var, double val)
{
	if (st.is_declared(var)) error (var, "declared twice");
	var_table.push_back(Variable(var,val));
	return val;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
		t = ts.get();
		if (t.kind != ')') error("')' expected");
        return d;
	}
	case '-':
		return - primary();
	case number:
		return t.value;
	case name:
		return st.get(t.name);
    case square_root:
        {
            double d = primary();
            if (d < 0) error("Can't sqrt() Imaginary #");
            return sqrt(d);
        }
	default:
		error("primary expected");
	}
}

double pow(double base, double exp)
{
    int x = narrow_cast<int>(exp);

    double n = 1;
    double b;

    if (exp < 0) {
        b = 1 / base;
        x *= -1;
    } else
        b = base;

    for (int i = 0; i < x; ++i)
        n *= b;

    return n;
}

double secondary()
{
    double left = primary();
    while(true) {
        Token t = ts.get();
        switch(t.kind) {
            case '^':
                return pow(left, primary());
            default:
                ts.unget(t);
                return left;
        }
    }
}

double term()
{
	double left = secondary();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '*':
			left *= secondary();
			break;
		case '/':
		{	
            double d = secondary();
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name) error ("name expected in declaration");
	string name = t.name;           // name already defined in outer scope

	if (st.is_declared(name)) error(name, " declared twice");

	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of " ,name);

	double d = expression();
	//var_table.push_back(Variable(name,d));
	st.define_name(name,d);
	return d;
}

double const_declaration()
{
	Token t = ts.get();
	if (t.kind != name) error ("name expected in declaration");
	string name = t.name;           // name already defined in outer scope

	if (st.is_declared(name)) error(name, " declared twice");

	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of " ,name);

	double d = expression();
	st.var_table.push_back(Variable(name,d,true));
	return d;
}

double statement()
{
	Token t = ts.get();
	switch(t.kind) {
	case let:
		return declaration();
	case Const:
		return const_declaration();
	case name:
		if(st.is_declared(t.name))
		{
			Token t1=t;
			char ch;
			cin.get(ch);

			if (ch == print || ch == '\n')
				return st.get(t1.name);

			if (ch == '='){
				t = ts.get();
				Token t2 = t;
				if (t2.kind != number)
					error("expected number");
				st.set(t1.name, t2.value);
				return t.value;
			}
			else{

				cin.putback(ch);
				t1.value = st.get(t1.name);
				t1.kind = number ;
				ts.unget(t1);
				return expression();
			}
		}

	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
    // This iteration lacks a define_var function
    st.var_table.push_back(Variable("pi", 3.14159));
    st.var_table.push_back(Variable("e", 2.718281828));
    st.var_table.push_back(Variable("k", 1000));

	while(true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t=ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch(runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

	try {
		calculate();
		return 0;
	}
	catch (exception& e) {
		cerr << "exception: " << e.what() << endl;
		char c;
		while (cin >>c&& c!=';') ;
		return 1;
	}
	catch (...) {
		cerr << "exception\n";
		char c;
		while (cin>>c && c!=';');
		return 2;
	}

/* TESTING
 *
 * 1+2;
 * 5-4; 3+2; 6-10;
 * 1 / (3 + 8) * 10;
 * let x1 = 5;
 * let x2 = 16 / 4;
 * x * x2 * pi;
 *
 * 1**2;
 * 2+8
 * let f@tnum = 10;
 * x / y;
 * xx - 500;
 */