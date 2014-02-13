#include <GL/glew.h>
#include <QtGui>
#include "glviewer.h"
#include "computeshader.hpp"
#include "qttexture.hpp"
#include <QImage>

GlViewer::GlViewer(QWidget *pParent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), pParent)
{
	m_scene = NULL;
	m_center_x = m_center_y = 0.5;
	m_scale = 1.0;

	setAutoFillBackground(false);
}

void GlViewer::resizeGL(int width, int height) {
    qDebug() << "resizeGL " << width << " " << height;
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


    cg::ComputeShader shader;
    shader.loadFromFile("../shader/test_image.cs");
    //shader.createBuffer<float>({0., 1., 2., 3., 4., 5., 6., 7., 8., 9.});
    //shader.bindBuffer();
    // GLfloat *test = new GLfloat[10];
    // shader.getBuffer(test);  
    //std::cout << "Result: " << std::endl;
    //for(int i=0; i<10; i++) {
    //    cout << test_res[i] << ", ";
    //}
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    shader.enable();
    GLuint tex_id = shader.genTexture();
    shader.bindImageTexture("destTex", tex_id); 
    glDispatchCompute( 512/16, 512/16, 1 ); // 512^2 threads in blocks of 16 
    //glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    glGenVertexArrays(1, &vao);

    fullscreenShader = new cg::Shader();
    fullscreenShader->loadVertexShaderFromFile("../shader/empty.vert");
    fullscreenShader->loadGeometryShaderFromFile("../shader/fullscreen_quad.geom");
    fullscreenShader->loadFragmentShaderFromFile("../shader/fullscreen_quad.frag");
    fullscreenShader->enable();
    fullscreenShader->setTexture("Texture", tex_id);

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
