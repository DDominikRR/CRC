//#include <bits/stdc++.h>

//includes
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <algorithm>

//strings
std::string polynomial = "1110111";
std::string bit_string = ""; //jesli 0 i 1 od uzytkownika, to wpisac tu i zmienic GET_FROM_FILE na 0
std::string result_string = "";
std::string bits_to_check = "";

//ints
int polynomial_length;
int bit_string_length;
int result_string_length;
int eights_needed = 0;

//settings
#define LOG 1
#define GET_FROM_FILE 1
#define CALCULATE_CRC 0
#define CHECK_CRC 1

//---FILES---
//inputs
char *INPUT_FILE_NAME = "crcinput.txt";

//other
char* LOG_FILE_NAME = "CRC_LOG.txt";

//Log
std::ofstream out(LOG_FILE_NAME);   //przekierowanie strumienia
auto old_rdbuf = std::clog.rdbuf(); //Get rdbuf to reset value before exiting

void get_bit_string_from_file()
{
    bit_string ="";

    FILE *INPUT_FILE = fopen(INPUT_FILE_NAME, "rb");
        if(!INPUT_FILE)
            std::cerr<<"Blad w otwarciu pliku "<<INPUT_FILE_NAME<<std::endl;

    int num;
    int byte;

    std::string eight = "00000000";

    while((byte = getc(INPUT_FILE)) != EOF)
    {
        num = byte;
        eight = "00000000";
        int r;
        int i = 0;
        while(num > 0)
        {
            r = num % 2;
            if(r == 1)
                eight[7-i] = '1';
            num = num / 2;
            i++;
        }
        bit_string = bit_string + eight;
    }
    fclose(INPUT_FILE);
}

void manage_bit_result_string()
{
    if(LOG) std::clog<<"\nWejscie w manage_bit_result_string()\n";
    if(LOG) std::clog<<"bit_string without added zeros: "<<bit_string<<std::endl;

    polynomial_length = polynomial.length();
    //polynomial_length = 32;
    while(1) //usuniecie zer z przodu
    {
        if(polynomial[0] == '0') polynomial.erase(0,1);
        else break;
    }
    //polynomial_length = polynomial.length();

    for(int i = 0; i < polynomial_length; i++)//dodanie zer + dlugosc crc
    {
        result_string = result_string + "0";
        if(i == polynomial_length - 1) break;
        bit_string = bit_string + "0";
    }

    bit_string_length = bit_string.length();
    result_string_length = result_string.length();

    if(LOG)
    {
        std::clog<<"bit_string_length : "<<bit_string_length<<std::endl;
        std::clog<<"polynomial_length : "<<polynomial_length<<std::endl;
        std::clog<<"result_string_length: "<<result_string_length<<std::endl;
        std::clog<<"bit_string : "<<bit_string<<std::endl;
        std::clog<<"polynomial : "<<polynomial<<std::endl;
        std::clog<<"result_string: "<<result_string<<std::endl<<std::endl;
    }
}

void calculate_CRC()
{
    std::string bit_string_copy = bit_string;
    int bit_string_copy_length = bit_string_copy.length();
    int copied_to_result = 0;

    for(int j = 0; j < result_string_length && j < bit_string_length; j++) // ustawiamy na result pierwsze do policzenia
    {
        result_string[j] = bit_string[j];
        copied_to_result++;
    }

    bit_string_copy.erase(0, copied_to_result);
    bit_string_copy_length = bit_string_copy.length();

    while(bit_string_copy_length)// DO
    {
        //std::cout<<"Prev remainder: "<<result_string<<" "<<bit_string_copy<<std::endl;
        //std::cout<<"Polynomial    : "<<polynomial<<std::endl;
        for(int i = 0; i < result_string_length; i++)
        {
            if(result_string[i] == polynomial[i])
            {
                result_string[i] = '0';
            }
            else
            {
                result_string[i] = '1';
            }
        }
        //std::cout<<"XOR result    : "<<result_string<<std::endl;
        int zero_count = 0;
        int loop_count = 0;
        for(int i = 0; i < result_string_length; i++)
        {
            if(result_string[i] == '0') zero_count ++;
                else break;
        }
        if(zero_count > 0)
        {
            for(int i = 0; i < zero_count && i < bit_string_copy_length; i++)
            {
                result_string = result_string + bit_string_copy[i];
                loop_count++;
            }

            result_string.erase(0,loop_count);
            bit_string_copy.erase(0,loop_count);
        }
        bit_string_copy_length = bit_string_copy.length();

        //std::cout<<"Zeros erased  : "<<result_string<<std::endl<<std::endl;
    }

    //std::cout<<"Prev remainder: "<<result_string<<" "<<bit_string_copy<<std::endl;
    //std::cout<<"Polynomial    : "<<polynomial<<std::endl;
    for(int i = 0; i < result_string_length; i++)
        {
            if(result_string[i] == polynomial[i])
            {
                result_string[i] = '0';
            }
            else
            {
                result_string[i] = '1';
            }
        }
    //std::cout<<"XOR result    : "<<result_string<<std::endl<<std::endl;
    result_string.erase(0,1);
    std::cout<<"REMAINDER     : "<<result_string<<std::endl;
}
void append_crc()
{
    std::fstream file;
    std::string eight = "00000000";
    std::string remainder = result_string;

    char bitschar;
    int remainder_length;

    remainder_length = remainder.length();

    file.open(INPUT_FILE_NAME, std::ios::app);

    while(remainder_length > 0)
    {
        eight = "00000000";
        for(int i = 0; i < remainder_length && i < 8; i++)
        {
            eight[i] = remainder[i];
        }
        int tempBtD = 1;
        int decVal = 0;
        for (int i = 7; i >= 0; i--)
        {
            if (eight[i] == '1')
            {
                decVal += tempBtD;
            }
            tempBtD = tempBtD * 2;
        }
        bitschar = decVal;
        file<<bitschar;

        int actLen = remainder.length();
        if(actLen >= 8)
            remainder.erase(0,8);
        else
            remainder.erase(0,actLen);
        remainder_length = remainder.length();
        //std::cout<<eight<<std::endl;
    }
    file.close();
}

int calculate_bytes_to_read()
{
    int temp_len = polynomial.length() - 1;

    while(temp_len > 0)
    {
        eights_needed ++;
        if(temp_len >= 8) temp_len = temp_len -8;
            else temp_len = 0;
    }
    return eights_needed;
}

void get_CRC_from_file( )
{
    std::fstream file;
    std::string eight = "00000000";
    int num;

    file.open(INPUT_FILE_NAME, std::ios::in );
    file.seekg(-eights_needed, std::ios::end);
    char tempp[eights_needed];
    //unsigned char tempp2[eights_needed];
    file.read(tempp, eights_needed);
    for(int j = 0; j < eights_needed; j++)
    {
        num = tempp[j];
            if(num < 0)
            {
                num = 256 + num;
            }
        //std::cout<<num<<std::endl;
        eight = "00000000";
        int r;
        int i = 0;
        while(num > 0)
        {
            r = num % 2;
            if(r == 1)
                eight[7-i] = '1';
            num = num / 2;
            i++;
        }
        bits_to_check = bits_to_check + eight;

    }
}

void check_if_good_crc()
{
    for(int i = 0; i < polynomial.length() - 1; i++)
    {
        if(result_string[i] != bits_to_check[i])
        {
            std::cout<<"CRC is not correct";
            return;
        }
    }
    std::cout<<std::endl<<"CRC is correct"<<std::endl;
}

int main()
{
    std::clog.rdbuf(out.rdbuf());       // Set the rdbuf of clog
    if(!LOG) std::clog<<"Logs Off";
    if(CALCULATE_CRC)
    {
        if(GET_FROM_FILE)
            get_bit_string_from_file();
        manage_bit_result_string();
        calculate_CRC();
        append_crc();
    }

    if(CHECK_CRC)
    {
        get_bit_string_from_file();
        eights_needed = calculate_bytes_to_read();
            bit_string.erase(bit_string.length() - eights_needed * 8, eights_needed * 8);
        manage_bit_result_string();
        calculate_CRC();
        get_CRC_from_file();
        check_if_good_crc();
    }

    if(LOG) std::clog<<"\n\nkoniec\n";
    std::clog.rdbuf(old_rdbuf);     //Reset the rdbuf of clog.

    return 0;
}
