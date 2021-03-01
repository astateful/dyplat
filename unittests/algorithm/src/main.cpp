#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "astateful/algorithm/Generate/Loop.hpp"
#include "astateful/algorithm/Generate/Recurse.hpp"
#include "astateful/algorithm/Generate/Thread.hpp"

using namespace astateful;
using namespace astateful::algorithm;

TEST_CASE( "loop" )
{
  Context<std::string, std::string> context;
  Persist<std::string, std::string> persist;
  GenerateLoop<std::string, std::string> generate( persist );
}

TEST_CASE( "recurse" )
{
  Context<std::string, std::string> context;
  Persist<std::string, std::string> persist;
  GenerateRecurse<std::string, std::string> generate( persist );
}

TEST_CASE( "thread" )
{
  Context<std::string, std::string> context;
  Persist<std::string, std::string> persist;
  GenerateThread<std::string, std::string> generate( persist );
}

