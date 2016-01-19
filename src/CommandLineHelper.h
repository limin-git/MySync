#pragma once
#include "Parameter.h"


struct CommandLineHelper
{
public:

    CommandLineHelper( int argc, char* argv[] );

    ParameterPtr get_parameter() { return m_parameter; }

private:

    void display_usage();

private:

    ParameterPtr m_parameter;
};
