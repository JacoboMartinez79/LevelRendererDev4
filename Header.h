///#pragma once
///#include <iostream>
///#include <fstream>
///#include <vector>
///#include "Gateware.h"
///
///std::ifstream input;
///std::vector<GW::MATH::GMATRIXF> world;
///std::vector<std::string> names;
///std::string filename = "../GameLevel.txt";
///std::string ending = ".H2B";
///
///bool FileIO()
///{
///	input.open(filename);
///
///	if (input.is_open())
///	{
///		while (!input.eof())
///		{
///			std::string buffer;
///			std::getline(input, buffer);
///			if (std::strcmp("MESH", buffer.c_str()) == 0)
///			{
///				std::string starting = "../Assets/";
///				std::getline(input, buffer);
///				buffer = buffer.substr(0, buffer.find("."));
///				buffer.append(ending);
///				starting += buffer;
///				names.push_back(starting);
///				GW::MATH::GVECTORF rows[4];
///				for (int i = 0; i < 4; i++)
///				{
///					std::getline(input, buffer);
///					buffer = buffer.substr(buffer.find('(') + 1, buffer.find(')') - buffer.find('(') - 1);
///
///					rows[i].x = atof(buffer.c_str());
///
///					buffer = buffer.substr(buffer.find(',') + 1);
///
///					rows[i].y = atof(buffer.c_str());
///
///					buffer = buffer.substr(buffer.find(',') + 1);
///
///					rows[i].z = atof(buffer.c_str());
///
///					buffer = buffer.substr(buffer.find(',') + 1);
///
///					rows[i].w = atof(buffer.c_str());
///				}
///				world.push_back(GW::MATH::GMATRIXF{ rows[0], rows[1], rows[2], rows[3] });
///
///			}
///
///		}
///		input.close();
///
///		for (int i = 0; i < world.size(); i++)
///		{
///			std::cout << names[i] << std::endl;
///			std::cout << world[i].row1.x << ", ";
///			std::cout << world[i].row1.y << ", ";
///			std::cout << world[i].row1.z << ", ";
///			std::cout << world[i].row1.w << std::endl;
///			std::cout << world[i].row2.x << ", ";
///			std::cout << world[i].row2.y << ", ";
///			std::cout << world[i].row2.z << ", ";
///			std::cout << world[i].row2.w << std::endl;
///			std::cout << world[i].row3.x << ", ";
///			std::cout << world[i].row3.y << ", ";
///			std::cout << world[i].row3.z << ", ";
///			std::cout << world[i].row3.w << std::endl;
///			std::cout << world[i].row4.x << ", ";
///			std::cout << world[i].row4.y << ", ";
///			std::cout << world[i].row4.z << ", ";
///			std::cout << world[i].row4.w << std::endl << std::endl;
///		}
///		return true;
///	}
///	else
///	{
///		std::cout << "Filename not found";
///		return false;
///	}
///}
/////from C++ Cookbook by D.Ryan Stephens, Christopher Diggins, Jonathan Turkanis, Jeff Cogswell
///// https://www.oreilly.com/library/view/c-cookbook/0596007612/ch10s17.html
/////void replaceExt(string& s, const string& newExt)
/////{
/////
/////    string::size_type i = s.rfind('.', s.length());
/////
/////    if (i != string::npos) {
/////        s.replace(i + 1, newExt.length(), newExt);
/////    }
/////}