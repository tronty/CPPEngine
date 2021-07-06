/*
 * container_widget.cpp
 *  Last Change: 06.05.2018
 *  Author: manitoo
 */

#include "container_widget.h"

//Constructor
Container_Widget::Container_Widget(QWidget *parent, GLuint Resolution) : QWidget(parent)
{
    qDebug() << "   [ Container_Widget constructed ]";

    clockwidget    = new Clock_Widget();

    glwidget       = new GL_Widget(0,Resolution);   //--> MainWindow's Central Widget
    glwidget->setMinimumSize(2300,1250);

    textedit        = new QTextEdit();
    textedit->setFixedHeight(210);

    QHBoxLayout* hLayout     = new QHBoxLayout(this);
    QVBoxLayout* vLayout     = new QVBoxLayout();

    //Group Boxes
    QGroupBox* groupbox_1   = new QGroupBox();
    groupbox_1->setFont(QFont("Helvetica",12));
    groupbox_1->setTitle("GL Actions");
    groupbox_1->setAlignment(Qt::AlignHCenter);
    groupbox_1->setBackgroundRole(QPalette::Dark);
    groupbox_1->setAutoFillBackground(true);

    QGroupBox* groupbox_2   = new QGroupBox();
    groupbox_2->setFont(QFont("Helvetica",12));
    groupbox_2->setTitle("Const Level Sets");
    groupbox_2->setAlignment(Qt::AlignHCenter);
    groupbox_2->setBackgroundRole(QPalette::Dark);
    groupbox_2->setAutoFillBackground(true);

    QGroupBox* groupbox_3   = new QGroupBox();
    groupbox_3->setFont(QFont("Helvetica",12));
    groupbox_3->setTitle("Scanning Mode");
    groupbox_3->setAlignment(Qt::AlignHCenter);
    groupbox_3->setBackgroundRole(QPalette::Dark);
    groupbox_3->setAutoFillBackground(true);

    textedit->adjustSize();
    textedit->setFont(QFont("Helvetica",12));
    textedit->setText(" ---------  Info  --------- \n");
    textedit->setAlignment(Qt::AlignHCenter);
    textedit->append(" [ --  Energy-Level-Sets  -- ]\n");
    textedit->append(" Ideal Thread-Count = "+QString::number(QThread::idealThreadCount())+"\n" );
    textedit->append("  Resolution = "+QString::number(glwidget->iDataSetSize)+"\n");
    textedit->append("  Step Size  = "+QString::number(glwidget->fStepSize)+"\n");
    textedit->append("  - Vertex     Shader\n");
    textedit->append("  - Fragment Shader\n");

    //Grids
    QGridLayout* grid_1       = new QGridLayout(groupbox_1);
    QGridLayout* grid_2       = new QGridLayout(groupbox_2);
    QGridLayout* grid_3       = new QGridLayout(groupbox_3);

    //GL Actions
    QRadioButton* Animate       = new QRadioButton;  Animate->setText("Animate -/+");
    QRadioButton* Shading       = new QRadioButton;  Shading->setText("Shading -/+");
    QRadioButton* Blending      = new QRadioButton;  Blending->setText("Blending -/+");
    QRadioButton* Culling       = new QRadioButton;  Culling->setText("Culling -/+");
    QRadioButton* Projection    = new QRadioButton;  Projection->setText("Projection");

    connect(Animate,SIGNAL(pressed()),glwidget,SLOT(animate()));
    connect(Shading,SIGNAL(pressed()),glwidget,SLOT(shading()));
    connect(Blending,SIGNAL(pressed()),glwidget,SLOT(blending()));
    connect(Culling,SIGNAL(pressed()),glwidget,SLOT(culling()));
    connect(Projection,SIGNAL(pressed()),glwidget,SLOT(projection()));

    grid_1->addWidget(Animate,    0,0);
    grid_1->addWidget(Shading,    1,0);
    grid_1->addWidget(Blending,   2,0);
    grid_1->addWidget(Culling,    3,0);
    grid_1->addWidget(Projection, 4,0);

    //Level Sets
    QRadioButton* Cavity_1      = new QRadioButton(groupbox_2); Cavity_1->setText("Cavity 1");
    QRadioButton* Cavity_2      = new QRadioButton(groupbox_2); Cavity_2->setText("Cavity 2");
    QRadioButton* Hydrogen_1    = new QRadioButton(groupbox_2);  Hydrogen_1->setText("Hydrogen 1");
    QRadioButton* Hydrogen_2    = new QRadioButton(groupbox_2);  Hydrogen_2->setText("Hydrogen 2");

    connect(Cavity_1,  SIGNAL(pressed()),glwidget,SLOT(LS1(void)));
    connect(Cavity_2,  SIGNAL(pressed()),glwidget,SLOT(LS2(void)));
    connect(Hydrogen_1,SIGNAL(pressed()),glwidget,SLOT(LS3(void)));
    connect(Hydrogen_2,SIGNAL(pressed()),glwidget,SLOT(LS4(void)));

    grid_2->addWidget(Cavity_1,  0,0);
    grid_2->addWidget(Cavity_2,  1,0);
    grid_2->addWidget(Hydrogen_1,2,0);
    grid_2->addWidget(Hydrogen_2,3,0);

    //Scanning Mode
    QRadioButton* Cubes         = new QRadioButton;  Cubes->setText("Cubes");
    QRadioButton* Tetrahedrons  = new QRadioButton;  Tetrahedrons->setText("Tetrahedrons");

    connect(Cubes,        SIGNAL(pressed()),glwidget,SLOT(CUBES(void)));
    connect(Tetrahedrons, SIGNAL(pressed()),glwidget,SLOT(TETRAHEDRONS(void)));

    grid_3->addWidget(Cubes,        0,0);
    grid_3->addWidget(Tetrahedrons, 1,0);

    QLabel* boostLabel_1         = new QLabel();
    boostLabel_1->setPixmap(QPixmap(":/Images/opengl-4-logo.gif"));
    QLabel* boostLabel_2         = new QLabel();
    boostLabel_2->setPixmap(QPixmap(":/Images/boost_small.png"));
    QLabel* macLabel_3         = new QLabel();
    macLabel_3->setPixmap(QPixmap(":/Images/PoweredByMacOSX.gif"));

    vLayout->addWidget(boostLabel_1);
    vLayout->addWidget(groupbox_1);
    vLayout->addWidget(groupbox_2);
    vLayout->addWidget(groupbox_3);
    vLayout->addWidget(textedit);
    vLayout->addWidget(boostLabel_2);
    vLayout->addWidget(clockwidget);
    vLayout->addWidget(macLabel_3);

    hLayout->addLayout(vLayout);
    hLayout->addWidget(glwidget);
}

//Destructor
Container_Widget::~Container_Widget()
{
    delete glwidget;
    qDebug() << "   [ Container_Widget destructed ]";
}

