#include <Model/Model.hpp>
#include <Renderer/Pipeline.hpp>

Model::Model(const std::string& fname, std::shared_ptr<Pipeline> pipeline) : fname(fname), pipeline(pipeline) {
	
}
