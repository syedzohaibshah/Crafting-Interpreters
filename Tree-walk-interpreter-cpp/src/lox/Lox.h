#ifndef LOX_HPP
#define LOX_HPP
#include<string>

class Lox{

    public:
static bool had_error;


void run_prompt();
void run_file(const std::string &path);
void run(const std::string& source);


static void error(int line,const std::string &message);
static void report(int line , const std::string &where, const std::string &message );
};


#endif
