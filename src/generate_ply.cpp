#include<fstream>
#define GLM_FORCE_RADIANS
#include<glm/glm.hpp>
#include<glm/ext.hpp>
#include<sstream>
//void cylinder(double x, double y, double z, double r, std::size_t& double ex, double ey, double ez)
void cylinder(const char* str, const glm::mat4& transform, const std::size_t& n_base, const size_t& n_h)
{
	std::ofstream ofs;
	ofs.open(str);
	ofs << "OFF" << std::endl;
	ofs << n_base*n_h << ' ' << 0 << ' ' << 0 << std::endl;
	const float& ratio = 1.0/(n_h-1);
	for(std::size_t i=0; i!=n_base; ++i)
	{
		const float& theta = i*2*M_PI/n_base;
		for(std::size_t j=0; j!=n_h; ++j)
		{
			const glm::vec4& tmp = (transform*glm::vec4(sin(theta),cos(theta),j*ratio,1));
			ofs << tmp.x << ' ' << tmp.y << ' ' << tmp.z << std::endl;
		}
	}
	ofs.close();
}
void cone(const char* str, const glm::mat4& transform, const std::size_t& n_base, const size_t& n_h)
{
	std::ofstream ofs;
	ofs.open(str);
	ofs << "OFF" << std::endl;
	ofs << n_base*n_h << ' ' << 0 << ' ' << 0 << std::endl;
	const float& ratio = 1.0/(n_h-1);
	const float& inv_n_h = 1.0/n_h;
	for(std::size_t i=0; i!=n_base; ++i)
	{
		const float& theta = i*2*M_PI/n_base;
		for(std::size_t j=0; j!=n_h; ++j)
		{
			const float& j_s_n_h = j*inv_n_h;
			const glm::vec4& tmp = (transform*glm::vec4(sin(theta)*(1-j_s_n_h),
														cos(theta)*(1-j_s_n_h),j*ratio,1));
			ofs << tmp.x << ' ' << tmp.y << ' ' << tmp.z << std::endl;
		}
	}
	ofs.close();
}
	
int main()
{
	glm::mat4 m;
	m*=0;
	for(auto i : {0,1,2,3})
		m[i][i]=1;
	m *= glm::scale(glm::vec3(1,1,4));
	cylinder("cylinder_1_1_4.off",m,500,500);
	cone("cone_1_1_4.off",m,500,500);
	m = glm::rotate(float(M_PI_2),glm::vec3(1,0,0))*m;
	cylinder("cylinder_1_4_1.off",m,500,500);
	cone("cone_1_4_1.off",m,500,500);
	m = glm::rotate(float(-M_PI_4),glm::vec3(1,0,0))*m;
	cylinder("cylinder_1_4_4.off",m,500,500);
	cone("cone_1_4_4.off",m,500,500);
	m*=0;
	for(auto i : {0,1,2,3})
		m[i][i]=1;
	m *= glm::scale(glm::vec3(1,1,1));
	cone("cone_1_1_1_z.off",m,500,500);
	m = glm::rotate(float(M_PI_4),glm::vec3(1,0,0))*m;
	cone("cone_1_1_1_zy.off",m,500,500);
	m = glm::rotate(float(M_PI_4),glm::vec3(1,0,0))*m;
	cone("cone_1_1_1_y.off",m,500,500);
	m*=0;
	for(auto i : {0,1,2,3})
		m[i][i]=1;
	m *= glm::scale(glm::vec3(1,1,.1));
	cone("cone_1_1_.1_z.off",m,500,500);
	m = glm::rotate(float(M_PI_4),glm::vec3(1,0,0))*m;
	cone("cone_1_1_.1_zy.off",m,500,500);
	m = glm::rotate(float(M_PI_4),glm::vec3(1,0,0))*m;
	cone("cone_1_1_.1_y.off",m,500,500);
	
	m*=0;
	for(auto i : {0,1,2,3})
		m[i][i]=1;
	m *= glm::scale(glm::vec3(1,1,.1));
	std::ostringstream oss;
	const std::size_t& n = 12;
	for(int i=0; i<n; i++)
	{
		oss.str("");
		oss << "cone_" << i << ".off";
		m = glm::rotate(float(i*M_PI_2/(n-1)),glm::vec3(1,0,0))*glm::scale(glm::vec3(1,1,.1));
		cone(oss.str().c_str(),m,500,500);
	}
}


