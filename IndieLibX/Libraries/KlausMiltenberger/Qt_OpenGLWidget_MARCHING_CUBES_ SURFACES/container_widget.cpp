/*
 * container_widget.cpp
 *  Last Changes: 06.05.2018
 *  Author: manitoo  Qt/C++ Coding
 */

#include "container_widget.h"

//Constructor
Container_Widget::Container_Widget(QWidget *parent, GLbyte Resolution) : QWidget(parent)
{
    qDebug() << "   [ Container_Widget constructed  | Main-TID: " << QThread::currentThreadId() << " ]";

    clock_widget    = new ClockWidget();
    gl_widget       = new GL_Widget(Resolution);   //Central Widget
    gl_widget->setMinimumSize(2300,1250);
    textedit        = new QTextEdit();
    textedit->setFixedHeight(210);

    QHBoxLayout* hLayout    = new QHBoxLayout(this);
    QVBoxLayout* vLayout    = new QVBoxLayout();

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
    groupbox_3->setTitle("Partition Mode");
    groupbox_3->setAlignment(Qt::AlignHCenter);
    groupbox_3->setBackgroundRole(QPalette::Dark);
    groupbox_3->setAutoFillBackground(true);

    textedit->adjustSize();
    textedit->setFont(QFont("Helvetica",12));
    textedit->setText(" -----------  Info  ----------- \n");
    textedit->setAlignment(Qt::AlignHCenter);
    textedit->append(" [ --  Math - Level - Sets  -- ]\n");
    textedit->append(" Ideal Thread-Count = "+QString::number(QThread::idealThreadCount())+"\n" );
    textedit->append(" * Resolution = "+QString::number(gl_widget->iDataSetSize)+"\n");
    textedit->append(" * Step Size  = "+QString::number(gl_widget->fStepSize)+"\n");
    textedit->append("  - Simple Vertex     Shader\n");
    textedit->append("  - Simple Fragment Shader\n");

    //Grids
    QGridLayout* grid_1       = new QGridLayout(groupbox_1);
    QGridLayout* grid_2       = new QGridLayout(groupbox_2);
    QGridLayout* grid_3       = new QGridLayout(groupbox_3);

    //GL Actions
    QRadioButton* Animate       = new QRadioButton;  Animate->setText("Animate - / +");
    QRadioButton* Shading       = new QRadioButton;  Shading->setText("Shading - / +");
    QRadioButton* Blending      = new QRadioButton;  Blending->setText("Blending - / +");
    QRadioButton* Culling       = new QRadioButton;  Culling->setText("Culling - / +");
    QRadioButton* Projection    = new QRadioButton;  Projection->setText("Projection");

    connect(Animate,   SIGNAL(pressed()), gl_widget,SLOT(animate()));
    connect(Shading,   SIGNAL(pressed()), gl_widget,SLOT(shading()));
    connect(Blending,  SIGNAL(pressed()), gl_widget,SLOT(blending()));
    connect(Culling,   SIGNAL(pressed()), gl_widget,SLOT(culling()));
    connect(Projection,SIGNAL(pressed()), gl_widget,SLOT(projection()));

    grid_1->addWidget(Animate,    0,0);
    grid_1->addWidget(Shading,    1,0);
    grid_1->addWidget(Blending,   2,0);
    grid_1->addWidget(Culling,    3,0);
    grid_1->addWidget(Projection, 4,0);

    //Level Sets
    QRadioButton* Potential     = new QRadioButton(groupbox_2); Potential->setText("Potential Surface");
    QRadioButton* Toroid        = new QRadioButton(groupbox_2); Toroid->setText("Toroidal  Surface");
    QRadioButton* DoubleTorus   = new QRadioButton(groupbox_2); DoubleTorus->setText("Double Torus");
    QRadioButton* Chmutov_1     = new QRadioButton(groupbox_2); Chmutov_1->setText("Chmutov_1 Surface");
    QRadioButton* Chmutov_2     = new QRadioButton(groupbox_2); Chmutov_2->setText("Chmutov_2 Surface");


    connect(Potential,  SIGNAL(pressed()), gl_widget,SLOT(LS1(void)));
    connect(Toroid,     SIGNAL(pressed()), gl_widget,SLOT(LS2(void)));
    connect(DoubleTorus,SIGNAL(pressed()), gl_widget,SLOT(LS3(void)));
    connect(Chmutov_1,  SIGNAL(pressed()), gl_widget,SLOT(LS4(void)));
    connect(Chmutov_2,  SIGNAL(pressed()), gl_widget,SLOT(LS5(void)));

    grid_2->addWidget(Potential,  0,0);
    grid_2->addWidget(Toroid,     1,0);
    grid_2->addWidget(DoubleTorus,2,0);
    grid_2->addWidget(Chmutov_1,  3,0);
    grid_2->addWidget(Chmutov_2,  4,0);

    //Scanning Mode
    QRadioButton* Cubes         = new QRadioButton;  Cubes->setText("Cubes");
    QRadioButton* Tetrahedrons  = new QRadioButton;  Tetrahedrons->setText("Tetrahedrons");

    connect(Cubes,        SIGNAL(pressed()), gl_widget,SLOT(CUBES(void)));
    connect(Tetrahedrons, SIGNAL(pressed()), gl_widget,SLOT(TETRAHEDRONS(void)));

    grid_3->addWidget(Cubes,        0,0);
    grid_3->addWidget(Tetrahedrons, 1,0);

    //Logos
    QLabel* label_1         = new QLabel();
    label_1->setPixmap(QPixmap(":/Images/opengl-4-logo.gif"));
    QLabel* label_2         = new QLabel();
    label_2->setPixmap(QPixmap(":/Images/logo.png"));
    QLabel* label_3         = new QLabel();
    label_3->setPixmap(QPixmap(":/Images/PoweredByMacOSX.gif"));

    //Vertical Layout (GL-Actions , Info , Logos)
    vLayout->addWidget(label_1);
    vLayout->addWidget(groupbox_1);
    vLayout->addWidget(groupbox_2);
    vLayout->addWidget(groupbox_3);
    vLayout->addWidget(textedit);
    vLayout->addWidget(label_2);
    vLayout->addWidget(clock_widget);
    vLayout->addWidget(label_3);

    //Horizontal Layout
    hLayout->addLayout(vLayout);
    hLayout->addWidget(gl_widget);
}


//Destructor
Container_Widget::~Container_Widget()
{
    delete clock_widget;
    delete textedit;
    delete gl_widget;
    qDebug() << "   [ Container_Widget   destructed ]";
}

