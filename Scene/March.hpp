#pragma once
/* file format for storing raymarched scene for dynamic SDF */
#include <Scene/Object.hpp>
#include <util/log.hpp>

#include <fstream>
#include <map>

namespace March {
	static std::vector<Object> readFile(const std::string& fname, std::map<std::string, int>& name_map) {
		/* this should be rewritten once we are loading more than 250 objects */
		std::ifstream in(fname);

		std::vector<Object> ret;
		Object tmp;
		std::string name;

		std::string type;
		int id = 0;

		while(in >> type) {
			Log::info("Object Type: %s\n", type.c_str());
			char dummy;
			in >> dummy;
			if (dummy != '(') {
				Log::error("Expected '(', found %c\n", dummy);
			}
			in >> tmp.center.x;
			in >> tmp.center.y;
			in >> tmp.center.z;
			in >> dummy;
			if (dummy != ')') {
				Log::error("Expected ')', found %c\n", dummy);
			}

			if (type == "SPHERE") {
				tmp.shape = Shape::eSPHERE;
				char dummy;
				in >> dummy;
				if (dummy != '(') {
					Log::error("Expected '(', found %c\n", dummy);
				}
				in >> tmp.dimensions.x;
				in >> dummy;
				if (dummy != ')') {
					Log::error("Expected ')', found %c\n", dummy);
				}
			} else if (type == "BOX") {
				tmp.shape = Shape::eBOX;
				char dummy;
				in >> dummy;
				if (dummy != '(') {
					Log::error("Expected '(', found %c\n", dummy);
				}
				in >> tmp.dimensions.x;
				in >> tmp.dimensions.y;
				in >> tmp.dimensions.z;
				in >> dummy;
				if (dummy != ')') {
					Log::error("Expected ')', found %c\n", dummy);
				}
			} else {
				Log::error("Expected type name, but not recognized %s!\n", type.c_str());
			}
			
			in >> name;
			if (name_map.find(name) == name_map.end())
				name_map[name] = id++;
			
			tmp.id = name_map[name];

			ret.push_back(tmp);
		}
		

		return ret;
	}
	
	static std::string find_name(const std::map<std::string, int>& map, const int id, bool& found) {
		found = false;
		std::string name = "##";
		for (const auto& p : map) {
			if (p.second == id) {
				found = true;
				name = p.first;
				break;
			}
		}

		return name;
	}

	static void add_sphere(const std::string& name, glm::vec3 center, float rad, Renderer* ren) {
		unsigned int id = 0xDEADBEEF;
		if (ren->scene_map.find(name) == ren->scene_map.end()) {
			/* start iterating from the number of total objects, which will break immediately if stuff is deleted and new things added before saving, so please change this */
			id = ren->scene_map.size();
			ren->scene_map[name] = id;
		} else {
			id = ren->scene_map[name];
		}

		ren->objects.push_back(Object{
			.center = glm::vec4(center.x, center.y, center.z, 0.0),
			.dimensions = glm::vec4(rad),
			.id = id,
			.shape = eSPHERE,
			});
	}

	static void add_box(const std::string& name, glm::vec3 center, glm::vec3 dim, Renderer* ren) {
		unsigned int id = 0xDEADBEEF;
		if (ren->scene_map.find(name) == ren->scene_map.end()) {
			/* start iterating from the number of total objects, which will break immediately if stuff is deleted and new things added before saving, so please change this */
			id = ren->scene_map.size();
			ren->scene_map[name] = id;
		} else {
			id = ren->scene_map[name];
		}

		ren->objects.push_back(Object{
			.center = glm::vec4(center.x, center.y, center.z, 0.0),
			.dimensions = glm::vec4(dim.x, dim.y, dim.z, 0.0),
			.id = id,
			.shape = eBOX,
			});
	}

	static void writeFile(const std::string& fname, const std::vector<Object>& objs, const std::map<std::string, int>& map) {
		std::ofstream out(fname);
		const char* shape_names[] = {
				"SPHERE",
				"BOX",
				"PLANE",
		};

		int anon = 0;
		for (const auto& obj : objs) {
			out << shape_names[obj.shape] << " ( " << obj.center.x << " " << obj.center.y << " " << obj.center.z << " ) ( ";

			switch (obj.shape) {
			case eSPHERE:
				out << obj.dimensions.x << " ";
				break;
			case eBOX:
				out << obj.dimensions.x << " "
					<< obj.dimensions.y << " "
					<< obj.dimensions.z << " ";
				break;
			case ePLANE:
				out << obj.dimensions.x << " "
					<< obj.dimensions.y << " "
					<< obj.dimensions.z << " ";
				break;
			}

			bool found = false;
			auto name = find_name(map, obj.id, found);

			if (!found || name[0] == '#') {
				name = std::string("#") + std::to_string(anon++);
			}
			out << ") " << name << std::endl;
		}
		out.close();
	}
};

