#include <GL/glew.h>
#include <QtGui>
#include "glviewer.h"
#include "computeshader.hpp"
#include "qttexture.hpp"
#include <QImage>

#include <vector>
#include <glm/glm.hpp>
#include "Utils.h"

using namespace std;

void max_size()
{
    // Max values for dispatch = max number of work groups possible
    int max_compute_workgroup_count[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_compute_workgroup_count[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_compute_workgroup_count[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_compute_workgroup_count[2]);

    int max_compute_workgroup_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_compute_workgroup_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &max_compute_workgroup_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &max_compute_workgroup_size[2]);

    int max_compute_workgroup_invocations;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_workgroup_invocations);


    cout << "GL_MAX_COMPUTE_WORKGROUP_COUNT: (" << max_compute_workgroup_count[0] << ", " << max_compute_workgroup_count[1] << ", " << max_compute_workgroup_count[2] << ")" << endl;
    cout << "GL_MAX_COMPUTE_WORKGROUP_SIZE: (" << max_compute_workgroup_size[0] << ", " << max_compute_workgroup_size[1] << ", " << max_compute_workgroup_size[2] << ")" << endl;
    cout << "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS: " << max_compute_workgroup_invocations << endl;
}

GlViewer::GlViewer(QWidget *pParent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), pParent)
{
	m_scene = NULL;
	m_center_x = m_center_y = 0.5;
	m_scale = 1.0;

	setAutoFillBackground(false);
}

void GlViewer::resizeGL(int width, int height) {
	//glViewport(0, 0, width, height);
	glViewport(0, 0, width, height);
	double aspect_ratio = double(height) / double(width);
    
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-1.0, 1.0, -aspect_ratio, aspect_ratio, -1.0, 1.0);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
}

void GlViewer::initializeGL() {
    glClearColor(1., 1., 1., 0.);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        throw std::runtime_error("Fuck you I'm drunk, and I'm gonna be drunk, till the next time I'm drunk!");
    }

    max_size();

    const int size = 1024*10;
    const float beta = 90.f;
    const float tex_res = (beta+1)*2;
    cout << "====================" << endl;
    cout << "Parameters:" << endl;
    cout << "\tBeta: " << beta << endl;
    cout << "\tTexture resolution: " << tex_res << endl;
    cout << "====================" << endl;

    std::vector<glm::vec4> normals(size);
    for(int i=0; i<size; i++) {
        normals[i] = glm::vec4(1., 1., 0., 0.);
    }

    cg::ComputeShader shader;
    shader.loadFromFile("../shader/gen_sampler_sphere.cs");
    GLuint buffer = shader.createBuffer(normals);
    cout << "Buffer id: " << buffer << endl;
    GLuint tex_id_north = shader.genTexture(tex_res, tex_res);
    GLuint tex_id_south = shader.genTexture(tex_res, tex_res);
    //cout << "Tex id: " << tex_id_north <<", " << tex_id_south << endl;
    shader.enable();
    shader.setUniform("beta", beta);
    shader.bindImageTexture("north_hemisphere", tex_id_north);
    shader.bindImageTexture("south_hemisphere", tex_id_south);

    shader.bindBuffer("MyBuffer", buffer, 2);
    glErr();

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    // Create groups of 1024 normals for computation
    // XXX: should ensure that group size is < GL_MAX_COMPUTE_WORK_GROUP_COUNT 
    glDispatchCompute( normals.size()/1024.f, 1, 1 ); 
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glm::vec4 *test_res = shader.getBuffer<glm::vec4>(buffer, size);  
    //glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glErr();
    std::cout << "Result: " << std::endl;
    for(int i=0; i<size; i++) {
        cout << test_res[i] << ", ";
    }


    glGenVertexArrays(1, &vao);
    // Test fractal shader
    //cg::ComputeShader fractal_shader;
    //fractal_shader.loadFromFile("../shader/test_image.cs");
    //fractal_shader.enable();
    //GLuint tex_id = shader.genTexture(512, 512);
    //fractal_shader.bindImageTexture("destTex", tex_id); 
    //glDispatchCompute( 512/16, 512/16, 1 ); // 512^2 threads in blocks of 16 

    fullscreenShader = new cg::Shader();
    fullscreenShader->loadVertexShaderFromFile("../shader/empty.vert");
    fullscreenShader->loadGeometryShaderFromFile("../shader/fullscreen_quad.geom");
    fullscreenShader->loadFragmentShaderFromFile("../shader/fullscreen_quad.frag");
    fullscreenShader->enable();
    fullscreenShader->setTexture("Texture", tex_id_north);
}

void GlViewer::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (!m_scene) return;

    fullscreenShader->enable();
    glBindVertexArray( vao );
    glDrawArrays( GL_POINTS, 0, 1 );
    glBindVertexArray(0);
}

void GlViewer::wheelEvent(QWheelEvent *event) {
	if (!m_scene) return;
	m_scale += 0.05 * (event->delta() / 120);
	if (m_scale <= 0.0) m_scale = 0.0;
	updateGL();
}

void GlViewer::mousePressEvent(QMouseEvent *event) {
	if (!m_scene) return;
	m_mouse_click = event->pos();

	if (event->button() == Qt::LeftButton) 
	{
		setCursor(QCursor(Qt::PointingHandCursor));
		sample_mouse_path(m_mouse_click);
	} 
	else 
	{
		setCursor(QCursor(Qt::ClosedHandCursor));
	}
}

void GlViewer::mouseMoveEvent(QMouseEvent *event)
{
	if(!m_scene) return;    
	m_mouse_move = event->pos();

	if (event->buttons() == Qt::LeftButton)
	{
		if (m_mouse_move != m_mouse_click)
			sample_mouse_path(m_mouse_move);
	}
	else 
	{        
		move_camera(m_mouse_click, m_mouse_move);
	}

	m_mouse_click = m_mouse_move;
	updateGL();
}

void GlViewer::mouseReleaseEvent(QMouseEvent *event) {
	if (!m_scene) return;
	m_mouse_move = event->pos();

	if (event->button() == Qt::LeftButton) 
	{
		if (m_mouse_move != m_mouse_click)
			sample_mouse_path(m_mouse_move);
	} 
	else 
	{
		move_camera(m_mouse_click, m_mouse_move);
	}    

	m_mouse_click = m_mouse_move;
	setCursor(QCursor(Qt::ArrowCursor));
	updateGL();
}

void GlViewer::sample_mouse_path(const QPoint& point)
{
	double x, y;
	convert_to_world_space(point, x, y);

	//m_scene->set_mouse_pos(Scene::Point(x, y));
}

void GlViewer::move_camera(const QPoint& p0, const QPoint& p1)
{
	m_center_x -= double(p1.x() - p0.x()) / double(width());
	m_center_y += double(p1.y() - p0.y()) / double(height()); 
}

void GlViewer::convert_to_world_space(const QPoint& point, double &x, double &y)
{
	double aspect_ratio = double(height()) / double(width());

	x = double(point.x()) / double(width());
	x = (2.0*x - 1.0) / m_scale;
	x += m_center_x;

	y = 1.0 - double(point.y()) / double(height());
	y = (2.0*y - 1.0) * aspect_ratio / m_scale;
	y += m_center_y;
}
