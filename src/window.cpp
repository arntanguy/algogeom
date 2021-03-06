// Qt
#include <QtGui>
#include <QActionGroup>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>

#include "window.h"

#include <fstream>

MainWindow::MainWindow() : 
QMainWindow(), Ui_MainWindow(), 
maxNumRecentFiles(15), recentFileActs(15)
{
	setupUi(this);
    
	// init scene
	m_scene = new Scene;
	viewer->set_scene(m_scene);
    
    
	// Handling actions
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));

	//TODO
	//XXX
	on_actionOpen_PLY_triggered();
}

MainWindow::~MainWindow()
{
	delete m_scene;
}

void MainWindow::update()
{
	viewer->repaint();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
}



void MainWindow::on_actionOpen_PLY_triggered()
{
    // XXX: path
    //m_scene->loadPLY("../data/laser/maison_Mougins/Mougins_0.ply");
    //m_scene->compute_delaunay();
}



