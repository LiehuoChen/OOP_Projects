#ifndef CS540_INTERPOLATE_HPP
#define CS540_INTERPOLATE_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

namespace cs540{

struct WrongNumberOfArgs : public std::invalid_argument{
    WrongNumberOfArgs(std::string const &mesg)
        :std::invalid_argument(mesg)
    {}
};

template <typename T>
bool checkManipulator(T &mani)
{
    //std::cout << __func__ << "called\n";
    std::ostringstream temp;
    temp << mani;
    return std::string("") == temp.str();
}

//std::ostringstream final_output;
std::string Ipolate(const char * output, std::ostringstream &final_output)
{
    //std::cout << __func__ << " string called\n";
    while(*output)
    {
        //if(!strncmp(output, "\\", 1) &&  !strncmp(output + 1, "%", 1))
        if(*output == '\\' && *(output + 1) == '%')
        { 
            output++;
            final_output << *output++;
        }
        //else if (!strncmp(output,"%", 1))
        else if (*output == '%')
        {
            throw WrongNumberOfArgs("more % than arguments \n");
        } else 
        {
            final_output << *output++;
        }
    }
    //final_output << '\0';
    return final_output.str();
}

template <typename T, typename... Formats>
std::string Ipolate(const char * output, std::ostringstream
        &final_output, T &&value, Formats &&...format)
{
    //std::cout << __func__ << " called\n";
    if(checkManipulator<T>(value))
    {
        final_output << value;
        Ipolate(output, final_output, std::forward<Formats>(format)...);
        return final_output.str();
    }
    while (*output)
    {
        if (*output == '\\' && *(output + 1) == '%')
        {
            final_output << *output++ ;
            final_output << *output++ ;
        }
        else if (*output == '%')
        {
            final_output << value;
            Ipolate(output+1, final_output, std::forward<Formats>(format)...);
            return final_output.str();
        }
        final_output << *output++;
    }

    if(*output == '\0' && sizeof...(format) == 0)
    {
        
        if(checkManipulator<T>(value))
        {
            final_output << value;
            Ipolate(output, final_output, std::forward<Formats>(format)...);
        }
        throw WrongNumberOfArgs("more arguments than the %\n");
    }
    return final_output.str();
}

template <typename... Args>
std::string Interpolate(const char *str, Args &&...args)
{
    std::ostringstream final;

    //std::cout << __func__ << " called\n";
    return Ipolate(str, final, std::forward<Args>(args)...);
}
}

#endif

/*
int main()
{
    using namespace cs540;
    std::cout << Interpolate(R"(\)", R"(\)") << std::endl;
    std::cout << Interpolate(R"(%)", R"(\%)") << std::endl;
    std::cout << Interpolate(R"(\\)", R"(\\)") << std::endl;
}
*/
