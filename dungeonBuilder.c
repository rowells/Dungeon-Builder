
#include <GL/glut.h>    
//#include <GL/gl.h>	
//#include <GL/glu.h>	
#include <unistd.h>     
#include <stdio.h>     
#include <stdlib.h>     
#include <math.h>       
//Special thanks to NeHe tutorials. I received a lot of assistance from that site.
// These are definitions for various keys
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))
#define STAR_NUM 50

int imgText = 0;
int twinkle = 0;
int light = 0;           // lighting on/off
int specular  =   0;
int local     =   0;  // Local Viewer Model
float shinyGhost[1]; //shininess
int mode=0;
int gemSpin=0; //animation variable for gems
int ghostRot=0;
int ghostChew=0; //animation variable for monsters
int alpha = 100;
GLfloat fogColor[4] = {0.05,0.05,0.05,0.5};   // color for the fog
GLuint  typeFog[]= { GL_EXP, GL_EXP2, GL_LINEAR }; // Array of the types of fog
GLuint  fogs = 2;                         // Which fog type in use
GLfloat LightAmbient[]  = {0.1f, 0.1f, 0.1f, 1.0f}; 
GLfloat LightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat Specular[]  = {0,0,0,1}; 
GLfloat LightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};
GLfloat LightPosition1[] = {-1.25, 0.5, -1.5, 0};
GLfloat LightPosition2[] = {5.5, 0.5, -6, 0};
GLfloat LightPosition3[] = {3, 0.5, 3.5, 0};
GLfloat LightPosition4[] = {1.375, 0.5, -1, 0};
GLfloat LightPosition5[] = {1, 0.5, 3.5, 0};



/* The number of our GLUT window */
int window; 
int inc       =  10;  // ghost increment
int emission  =   0;  // Emission value


GLuint i;             //i variable for looping
int j;
int k;
GLuint texture[10];    // array of textures


GLfloat xrot;              // x angle of camera
GLfloat yrot=270;            // y angle of camera

GLfloat mov1 = 0;      //motion variables
GLfloat mov2 = 0;

GLfloat lookupdown = 0.0;
const float piover180 = 0.0174532925f;

float heading, xpos=-9, zpos;

GLfloat z=0.;                       // depth into the screen.


typedef struct {         // Coordinates of the vertices in the text file.
    GLfloat x, y, z;    
    GLfloat u, v;        
} VERTEX;

typedef struct {         // defines a triangle as an array of 3 vertices
    VERTEX vertex[3];    
} TRIANGLE;

typedef struct {         // BUILD of a 3d environment
    int totalWalls;    
    TRIANGLE* triangle;  
} BUILD;


BUILD BUILD1;

typedef struct {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
} Image;

// degrees to radians...2 PI radians = 360 degrees
float rad(float angle)
{
    return angle * piover180;
}

// This file enables my program to read data from a text file. This is important
// in the "buildMyDungeon" function.
void readstr(FILE *f, char *string)
{
    do {
	fgets(string, 255, f); // read the line
    } while ((string[0] == '/') || (string[0] == '\n'));
    return;
}

// This function allows me to store the wall layout of my dungeon in a textfile.
//This is useful because it enables me to make changes to the layout without having to recompile each time.

void buildMyDungeon() 
{
    float x, y, z, u, v;
    int vert;
    int totalWalls;
    FILE *filein;        
    char oneline[255];

    filein = fopen("Data/dungeonData/dungeon1.txt", "rt");//this is the file where the dungeon layout is stored.

    readstr(filein, oneline);
    sscanf(oneline, "NUMPOLLIES %d\n", &totalWalls);

    BUILD1.totalWalls = totalWalls;
    BUILD1.triangle = (TRIANGLE *) malloc(sizeof(TRIANGLE)*totalWalls);
    
    for (i = 0; i < totalWalls; i++) {
	for (vert = 0; vert < 3; vert++) {
	    readstr(filein,oneline);
	    sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
	    BUILD1.triangle[i].vertex[vert].x = x;
	    BUILD1.triangle[i].vertex[vert].y = y;
	    BUILD1.triangle[i].vertex[vert].z = z;
	    BUILD1.triangle[i].vertex[vert].u = u;
	    BUILD1.triangle[i].vertex[vert].v = v;
	}
    }

    fclose(filein);
    return;
}
   
/*The following I have learned from NeHe tutorials. It enables to run
 * with any kind of endianness. 
 */
 
static unsigned int getint(fp)
     FILE *fp;
{
  int c, c1, c2, c3;

  c = getc(fp);  
  c1 = getc(fp);  
  c2 = getc(fp);  
  c3 = getc(fp);
  
  return ((unsigned int) c) +   
    (((unsigned int) c1) << 8) + 
    (((unsigned int) c2) << 16) +
    (((unsigned int) c3) << 24);
}

static unsigned int getshort(fp)
     FILE *fp;
{
  int c, c1;
  
  c = getc(fp);  
  c1 = getc(fp);

  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}


//This is a method of loading bitmaps that I learned from NeHe tutorials.
int ImageLoad(char *filename, Image *image) 
{
    FILE *file;
    unsigned long size;                 
    unsigned long i;                    
    unsigned short int planes;           
    unsigned short int bpp;             
    char temp;                          

    if ((file = fopen(filename, "rb"))==NULL) {
      printf("File Not Found : %s\n",filename);
      return 0;
    }
    fseek(file, 18, SEEK_CUR);
    image->sizeX = getint (file);
    image->sizeY = getint (file);
    size = image->sizeX * image->sizeY * 3;
    planes = getshort(file);
    if (planes != 1) {
	return 0;
    }
    bpp = getshort(file);
    if (bpp != 24) {
      return 0;
    }
	fseek(file, 24, SEEK_CUR);
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	return 0;	
    }
    if ((i = fread(image->data, size, 1, file)) != 1) {
	return 0;
    }

    for (i=0;i<size;i+=3) { 
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }
    return 1;
}


//Here we are taking the bitmaps for use in our textures
GLvoid LoadGLTextures(GLvoid) 
{	

    Image *image1;
    
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/cobblestone.bmp", image1)) {
	exit(1);
    }      
      
    Image *image2;
    
    image2 = (Image *) malloc(sizeof(Image));
    if (image2 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/ceilingnfloor.bmp", image2)) {
	exit(1);
    }  
    
    Image *image3;
    
    image3 = (Image *) malloc(sizeof(Image));
    if (image3 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/gem.bmp", image3)) {
	exit(1);
    }  
    
    Image *image4;
    
    image4 = (Image *) malloc(sizeof(Image));
    if (image4 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/fire.bmp", image4)) {
	exit(1);
    }  
    
    Image *image5;
    
    image5 = (Image *) malloc(sizeof(Image));
    if (image5 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/Ghost.bmp", image5)) {
	exit(1);
    }  
    
    Image *image6;
    
    image6 = (Image *) malloc(sizeof(Image));
    if (image6 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/eye.bmp", image6)) {
	exit(1);
    }   
       
    Image *image7;
    
    image7 = (Image *) malloc(sizeof(Image));
    if (image7 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/teeth.bmp", image7)) {
	exit(1);
    } 
    
    Image *image8;
    
    image8 = (Image *) malloc(sizeof(Image));
    if (image8 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/Star.bmp", image8)) {
	exit(1);
    } 
      
    Image *image9;
    
    image9 = (Image *) malloc(sizeof(Image));
    if (image9 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/brain.bmp", image9)) {
	exit(1);
    }   
    
    Image *image10;
    
    image10 = (Image *) malloc(sizeof(Image));
    if (image9 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("Data/dungeonData/scales.bmp", image10)) {
	exit(1);
    }     
  
    glGenTextures(10, &texture[0]);

    // Image 1- cobblestone texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

    // Image 2- Dark stone texture
    glBindTexture(GL_TEXTURE_2D, texture[1]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);

    // Image 3- Gem texture
    glBindTexture(GL_TEXTURE_2D, texture[2]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image3->data);
    
    // Image 4- Fire texture
    glBindTexture(GL_TEXTURE_2D, texture[3]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image4->sizeX, image4->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image4->data); 
       
    // Image 5- Ghost texture
    glBindTexture(GL_TEXTURE_2D, texture[4]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image5->sizeX, image5->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image5->data);    
    
    // Image 6- eye texture
    glBindTexture(GL_TEXTURE_2D, texture[5]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image6->sizeX, image6->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image6->data);    

   // Image 7- teeth texture
    glBindTexture(GL_TEXTURE_2D, texture[6]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image7->sizeX, image7->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image7->data); 
       
    // Image 8- warp texture
    glBindTexture(GL_TEXTURE_2D, texture[7]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image8->sizeX, image8->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image8->data);  
    
     // Image 9- brain texture
    glBindTexture(GL_TEXTURE_2D, texture[8]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image9->sizeX, image9->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image9->data);  
    
    // Image 10- scale texture
    glBindTexture(GL_TEXTURE_2D, texture[9]);   
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image10->sizeX, image10->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image10->data);  


};

// This function initializes some GL functionality.

GLvoid GLStuff(GLsizei Width, GLsizei Height)	
{
    LoadGLTextures();                           
    glEnable(GL_TEXTURE_2D);                   

    glClearColor(0.0, 0.0, 0.0, 0.0);	
    glClearDepth(1.0);				
    glDepthFunc(GL_LESS);                       
    glEnable(GL_DEPTH_TEST); 
                       
    glShadeModel(GL_SMOOTH);			
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				
    
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	
    
    glMatrixMode(GL_MODELVIEW);

    // set up lights.
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

 
 /*   
    GLfloat LightDiffuse1[]  = {0, 0.25, 0, 1}; 
	GLfloat LightPosition1[] = {1, 0.5, -2.5, 0};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse1);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition1);
    //glEnable(GL_LIGHT1);

	GLfloat LightDiffuse2[]  = {0, 0.25, 0, 1}; 
	GLfloat LightPosition2[] = {1.25, 0.5, -1, 0};
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuse2);
	glLightfv(GL_LIGHT2, GL_POSITION, LightPosition2);
	glEnable(GL_LIGHT2);

	GLfloat LightDiffuse3[]  = {0, 0.25, 0, 1}; 
	GLfloat LightPosition3[] = {-1, 0.5, -1, 0};
	glLightfv(GL_LIGHT3, GL_DIFFUSE, LightDiffuse3);
	glLightfv(GL_LIGHT3, GL_POSITION, LightPosition3);
	glEnable(GL_LIGHT3);

	GLfloat LightDiffuse4[]  = {0, 0.25, 0., 1}; 
	GLfloat LightPosition4[] = {-5, 0.5, -7, 0};
	glLightfv(GL_LIGHT4, GL_DIFFUSE, LightDiffuse4);
	glLightfv(GL_LIGHT4, GL_POSITION, LightPosition4);
	glEnable(GL_LIGHT4);

	GLfloat LightDiffuse5[]  = {0, 0.25, 0, 1}; 
	GLfloat LightPosition5[] = {6, 0.5, -4, 0};
	glLightfv(GL_LIGHT5, GL_DIFFUSE, LightDiffuse5);
	glLightfv(GL_LIGHT5, GL_POSITION, LightPosition5);
	glEnable(GL_LIGHT5);
*/
    
   // glBlendFunc(GL_SRC_ALPHA,GL_ONE);			
/*
  glFogi(GL_FOG_MODE, GL_EXP);               // Fog initialization
  glFogfv(GL_FOG_COLOR, fogColor);               
  glFogf(GL_FOG_DENSITY, 0.75);                 
  glHint(GL_FOG_HINT, GL_NICEST);             
  glFogf(GL_FOG_START, 1.0f);                    
  glFogf(GL_FOG_END, 5.0f);                      
  glEnable(GL_FOG);                              
  */
  

    }    


GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
    if (Height==0)				
	Height=1;

    glViewport(0, 0, Width, Height);		
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0,(GLfloat)Width/(GLfloat)Height,0.1f,100.0);
    glMatrixMode(GL_MODELVIEW);
}
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME);
   gemSpin = fmod(t*0.1,360);
   ghostChew = t;

   glutPostRedisplay();
}
static void wormHead(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glEnable(GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   

   glBegin(GL_TRIANGLES);
 
//Left side of face
//glNormal3f( 0, 0, 1);
   glNormal3f(0, 2, 2);
   glTexCoord2f(0,0); glVertex3f(-1, 0, 0);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, 1, 0);
   glTexCoord2f(0,1); glVertex3f(0, 0, -1);

//Right side of face
   //glNormal3f( 0, 0, 1);
   glNormal3f(-2, 2, 0);
   glTexCoord2f(0,0); glVertex3f(-1, 0, 0);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, 1, 0);
   glTexCoord2f(0,1); glVertex3f(0, 0, 1);
   

//bottom
   //glNormal3f( 0, 0, 1);
   glNormal3f(2, 2, 0);
   glTexCoord2f(0,0); glVertex3f(0, 0, -1);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(-1, 0, 0);
   glTexCoord2f(0,1); glVertex3f(0, 0, 1);
   
   

   glEnd();
   glPopMatrix();
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);



}
static void wormSegment(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   
   glBegin(GL_QUADS);

   glColor3f(0.5,0.5,0.5);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-0.5, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-0.5, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+0.5, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+0.5, 1);
   

   glNormal3f( 0, 0,-1);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,-0.5,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-0.5,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+0.5,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+0.5,-1);

   glNormal3f(+1, 0, 0);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,-0.5,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-0.5,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+0.5,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+0.5,+1);

   glNormal3f(-1, 0, 0);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-0.5,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-0.5,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+0.5,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+0.5,-1);
   
   //top
   glNormal3f( 0,+1, 0);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,+0.5,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+0.5,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+0.5,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+0.5,-1);

   glNormal3f( 0,-1, 0);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-0.5,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-0.5,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-0.5,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-0.5,+1);
   
   glEnd();
   glBegin(GL_TRIANGLES);
   //left legs *from front perspecive
   glNormal3f( 0, 2, -1.8);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,-0.5,-1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,-2);
   glTexCoord2f(1,1); glVertex3f(-1,-0.5,-1);
   
   glNormal3f( -2.4, 2, -1.4);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,0.5,-1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,-2);
   glTexCoord2f(1,1); glVertex3f(-1,0.5,-1);
   
   glNormal3f( -1, 0, 1);
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,-0.5,-1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,-2);
   glTexCoord2f(1,1); glVertex3f(+1,0.5,-1); 

   glNormal3f( 1, 0, 1);   
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-0.5,-1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,-2);
   glTexCoord2f(1,1); glVertex3f(-1,0.5,-1);
   
   //right legs *from front perspecive
   glNormal3f(0, -2, -1.8);   
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,-0.5,1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,2);
   glTexCoord2f(1,1); glVertex3f(-1,-0.5,1);
   
   glNormal3f(0, -2, -3.8);   
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,0.5,1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,2);
   glTexCoord2f(1,1); glVertex3f(-1,0.5,1);

   glNormal3f(1, 0, 1);      
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(+1,-0.5,1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,2);
   glTexCoord2f(1,1); glVertex3f(+1,0.5,1); 
   
   glNormal3f(1, 0, -1);      
   //glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-0.5,1);
   glTexCoord2f(0,1);glTexCoord2f(1,0); glVertex3f(0,-1.4,2);
   glTexCoord2f(1,1); glVertex3f(-1,0.5,1);
       
   glEnd();
   glPopMatrix();

}
/*
static void walls(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   
   glBegin(GL_QUADS);
   //top
   glColor3f(0.5,0.5,0.5);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   

   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);

   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);

   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);

   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);

   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   
   glEnd();
   glPopMatrix();

}
*/
static void tentacles(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   
   glBegin(GL_QUADS);
   //top
   glColor3f(0.5,0.5,0.5);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   
// glNormal3f( 0, 0, 1);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);

// glNormal3f( 0, 0, 1);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);

   //glNormal3f( 0, 0, 1);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);

// glNormal3f( 0, 0, 1);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
 
 //glNormal3f( 0, 0, 1);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   
   glEnd();
   glPopMatrix();

}
static void antennae(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   
   glBegin(GL_QUADS);
   //top
   glColor3f(0.5,0.5,0.5);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   

   glNormal3f( 0, 0,-1);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);

   glNormal3f(+1, 0, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);

   glNormal3f(-1, 0, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);

   glNormal3f( 0,+1, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);

   glNormal3f( 0,-1, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   
   glEnd();
   glPopMatrix();

}
static void ceilingsFloors(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   //glNormal3f( 0,0,1);

   glColor3f(0.5,0.5,0.5);
   glTexCoord2f(0,0); glVertex3f(-1, 1, 1);
   glTexCoord2f(1,0); glVertex3f(-1, 1, -1);
   glTexCoord2f(1,1); glVertex3f(1, 1, -1);
   glTexCoord2f(0,1); glVertex3f(1, 1, 1);
   
   glEnd();
   glPopMatrix();

} 
void mouthTop(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   
   glBegin(GL_TRIANGLES);
   glNormal3f(0, 0, 1);
//top Jaw
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-1, 1, 0.1);
   glTexCoord2f(1,0); glVertex3f(-0.75, -0.3, 0.1);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(-0.5, 1, 0.1);
   
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-0.5, 1, 0.1);
   glTexCoord2f(1,0); glVertex3f(-0.25, 0.75, 0.1);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(0, 1, 0.1);
   
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(0, 1, 0.1);
   glTexCoord2f(1,0); glVertex3f(0.25, 0.75, 0.1);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(0.5, 1, 0.1);
   
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(0.5, 1, 0.1);
   glTexCoord2f(1,0); glVertex3f(0.75, -0.3, 0.1);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(1, 1, 0.1);
   
   glEnd();
   glPopMatrix();
}

void mouthBottom(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   
   glBegin(GL_TRIANGLES);
   glNormal3f(0, 0, 1);
//bottom jaw
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-0.9, -0.1, 0);
   glTexCoord2f(1,0); glVertex3f(-0.75, 0.25, 0);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(-0.6, -0.1, 0);
   
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-0.6, -0.1, 0);
   glTexCoord2f(1,0); glVertex3f(-0.45, 0.25, 0);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(-0.3, -0.1, 0);
   
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-0.3, -0.1, 0);
   glTexCoord2f(1,0); glVertex3f(-0.15, 0.25, 0);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(0, -0.1, 0);
   
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(0, -0.1, 0);
   glTexCoord2f(1,0); glVertex3f(0.15, 0.25, 0);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(0.3, -0.1, 0);

   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(0.3, -0.1, 0);
   glTexCoord2f(1,0); glVertex3f(0.45, 0.25, 0);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(0.6, -0.1, 0);
   
      glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(0.6, -0.1, 0);
   glTexCoord2f(1,0); glVertex3f(0.75, 0.25, 0);
   glTexCoord2f(1,1);glTexCoord2f(0,1); glVertex3f(0.9, -0.1, 0);

  
   
   
   glEnd();
   glPopMatrix();

} 	                 
static void Vertex(double th2,double ph2, double tex1, double tex2)
{
   double x = Sin(th2)*Cos(ph2);
   double y = Cos(th2)*Cos(ph2);
   double z =         Sin(ph2);
   glNormal3d(x,y,z);
   //glNormal3f( 0,0,1);
   glTexCoord2f(tex1,tex2);
   glVertex3d(x,y,z);
}
static void ghost(double x,double y,double z,double r, int texVal)
{
   int th2,ph2;
   float ghostSpec[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(r,r,r);
   glEnable(GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T); 
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   glColor3f(0.5,0.5,0.5);
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT,GL_SPECULAR,ghostSpec);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   for (ph2=-90;ph2<90;ph2+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th2=0;th2<=360;th2+=inc)
      {
         Vertex(th2,ph2, th2, ph2);
         Vertex(th2,ph2+inc, th2, ph2+inc);
      }
      glEnd();
   }

   glPopMatrix();
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T); 
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);

}  

static void warpWall(double x,double y,double z, double dx,double dy,double dz, double th, int texVal)
{  
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   glColor3f(0.5,0.5,0.5);

	glBegin(GL_QUADS);	
	glNormal3f( 0,0,1);		
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0,-1.0, 0.0);
	glTexCoord2f(1.0, 0.0f); glVertex3f( 1.0,-1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f( 1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
	glEnd();			
	
   
   glPopMatrix();
}

static void brain(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   glColor3f(0.5,0.5,0.5);

   glBegin(GL_QUADS);
   
//top
   glNormal3f( 0,+1, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-0.25,+1,+0.5);
   glTexCoord2f(1,0); glVertex3f(+0.25,+1,+0.5);
   glTexCoord2f(1,1); glVertex3f(+0.25,+1,-0.5);
   glTexCoord2f(0,1); glVertex3f(-0.25,+1,-0.5);

//top to front connection
   glNormal3f( 0,-0.25, -0.25);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(-0.25,+1,+0.5);
   glTexCoord2f(1,0); glVertex3f(+0.25,+1,+0.5);
   glTexCoord2f(1,1); glVertex3f(+0.75,+0.5, 1);
   glTexCoord2f(0,1); glVertex3f(-0.75,+0.5, 1);
   
//top to right face
   glNormal3f( -0.5,-0.5, 0);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(+0.25,+1,+0.5);
   glTexCoord2f(1,0); glVertex3f(+0.25,+1,-0.5);
   glTexCoord2f(1,1); glVertex3f(+0.75,+0.5,-1);
   glTexCoord2f(0,1); glVertex3f(+0.75,+0.5,+1);
   
//top to left face
   glNormal3f( -0.5,0.5, 0);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(-0.25,+1,+0.5);
   glTexCoord2f(1,0); glVertex3f(-0.25,+1,-0.5);
   glTexCoord2f(1,1); glVertex3f(-0.75,+0.5,-1);
   glTexCoord2f(0,1); glVertex3f(-0.75,+0.5,+1);
   
//top to back face
   glNormal3f( 0,-1.25, 0.25);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(+0.25,+1,-0.5);
   glTexCoord2f(1,0); glVertex3f(-0.25,+1,-0.5);
   glTexCoord2f(1,1); glVertex3f(-0.5,+0.5,-3);
   glTexCoord2f(0,1); glVertex3f(+0.5,+0.5,-3);
   glEnd();
//right top to back gap
   glBegin(GL_TRIANGLES);
   glNormal3f( -1,-1.125, 0.125);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(+0.25,+1,-0.5);
   glTexCoord2f(0,1); glVertex3f(+0.5,+0.5,-3);
   glTexCoord2f(1,1); glVertex3f(+0.75,+0.5,-1);
   
//left top to back gap
   glNormal3f( -1,1.125, -0.125);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(-0.25,+1,-0.5);
   glTexCoord2f(0,1); glVertex3f(-0.5,+0.5,-3);
   glTexCoord2f(1,1); glVertex3f(-0.75,+0.5,-1);
   

   glEnd();
   glBegin(GL_QUADS);

   
//front face
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-0.75,-0.5, 1);
   glTexCoord2f(1,0); glVertex3f(+0.75,-0.5, 1);
   glTexCoord2f(1,1); glVertex3f(+0.75,+0.5, 1);
   glTexCoord2f(0,1); glVertex3f(-0.75,+0.5, 1);
   
//back face
   glNormal3f( 0, 0,-1);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(+0.5,-0.5,-3);
   glTexCoord2f(1,0); glVertex3f(-0.5,-0.5,-3);
   glTexCoord2f(1,1); glVertex3f(-0.5,+0.5,-3);
   glTexCoord2f(0,1); glVertex3f(+0.5,+0.5,-3);
//right face
   glNormal3f(+1, 0, 0);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(+0.75,-0.5,+1);
   glTexCoord2f(1,0); glVertex3f(+0.75,-0.5,-1);
   glTexCoord2f(1,1); glVertex3f(+0.75,+0.5,-1);
   glTexCoord2f(0,1); glVertex3f(+0.75,+0.5,+1);
   
// right face to back face
   glNormal3f(+4, 0, -0.25);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,1); glVertex3f(0.75,+0.5,-1);
   glTexCoord2f(0,0); glVertex3f(0.75,-0.5,-1);
   glTexCoord2f(1,0); glVertex3f(0.5,-0.5,-3);
   glTexCoord2f(1,1); glVertex3f(0.5,+0.5,-3);
   
//left face
   glNormal3f(-1, 0, 0);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(-0.75,-0.5,-1);
   glTexCoord2f(1,0); glVertex3f(-0.75,-0.5,+1);
   glTexCoord2f(1,1); glVertex3f(-0.75,+0.5,+1);
   glTexCoord2f(0,1); glVertex3f(-0.75,+0.5,-1);
   
// left face to back face
   glNormal3f(+4, 0, 0.25);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,1); glVertex3f(-0.75,+0.5,-1);
   glTexCoord2f(0,0); glVertex3f(-0.75,-0.5,-1);
   glTexCoord2f(1,0); glVertex3f(-0.5,-0.5,-3);
   glTexCoord2f(1,1); glVertex3f(-0.5,+0.5,-3);
   


//bottom
   glNormal3f( 0,-1, 0);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(0,0); glVertex3f(-0.75,-0.5,+0.5);
   glTexCoord2f(1,0); glVertex3f(+0.75,-0.5,+0.5);
   glTexCoord2f(1,1); glVertex3f(+0.75,-0.5,-0.5);
   glTexCoord2f(0,1); glVertex3f(-0.75,-0.5,-0.5);   
  


//bottom to back face
   glNormal3f( 0,-3.75, 0);
   //glNormal3f( 0,0, 1);
   glTexCoord2f(1,1); glVertex3f(+0.75,-0.5,-0.5);
   glTexCoord2f(0,1); glVertex3f(-0.75,-0.5,-0.5);
   glTexCoord2f(1,0); glVertex3f(-0.5,-0.5,-3);
   glTexCoord2f(0,0); glVertex3f(+0.5,-0.5,-3);
   glEnd();

      
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);

}
/*
static void cylinder(double x,double y,double z,double r)
{
	
   //int th2,ph2;
   float ghostSpec[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(r,r,r);
   glEnable(GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
   glEnable(GL_TEXTURE_GEN_S);
   glEnable(GL_TEXTURE_GEN_T); 
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glColor3f(0.5,0.5,0.5);
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT,GL_SPECULAR,ghostSpec);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //top octagon
   int sides = 20;  // The amount of segment to create the circle
 
glBegin(GL_LINE_LOOP);
 
  for (inc = 0; inc < 360; inc += 360 / sides)
  {
    double heading = inc * 3.1415926535897932384626433832795 / 180;
    glVertex2d(Cos(heading) * r, Sin(heading) * r);
  }
 
glEnd();
   glPopMatrix();
   glDisable(GL_TEXTURE_GEN_S);
   glDisable(GL_TEXTURE_GEN_T); 
   glDisable(GL_TEXTURE_2D);

}  
 */         
static void gem(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, int texVal)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyGhost);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glEnable(GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[texVal]);
   

   glBegin(GL_TRIANGLES);
 
//facet 1
   glNormal3f(0, 2, 2);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1, 0, 1);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, 1, 0);
   glTexCoord2f(0,1); glVertex3f(1, 0, 1);

//facet 2
   glNormal3f(-2, 2, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1, 0, 1);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, 1, 0);
   glTexCoord2f(0,1); glVertex3f(-1, 0, -1);
   
//facet 3
   glNormal3f(0, 2, -2);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(0, 1, 0);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(1, 0, -1);
   glTexCoord2f(0,1); glVertex3f(-1, 0, -1);

//facet 4
   glNormal3f(2, 2, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(1, 0, -1);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, 1, 0);
   glTexCoord2f(0,1); glVertex3f(1, 0, 1);
//Lower facets

//facet 1
   glNormal3f(0, -2, 2);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1, 0, 1);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, -1, 0);
   glTexCoord2f(0,1); glVertex3f(1, 0, 1);

//facet 2
   glNormal3f(-2, -2, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(-1, 0, 1);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, -1, 0);
   glTexCoord2f(0,1); glVertex3f(-1, 0, -1);
   
//facet 3
   glNormal3f(0, -2, -2);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(0, -1, 0);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(1, 0, -1);
   glTexCoord2f(0,1); glVertex3f(-1, 0, -1);

//facet 4
   glNormal3f(2, -2, 0);
   //glNormal3f( 0,0,1);
   glTexCoord2f(0,0); glVertex3f(1, 0, -1);
   glTexCoord2f(1,0); glTexCoord2f(1,1); glVertex3f(0, -1, 0);
   glTexCoord2f(0,1); glVertex3f(1, 0, 1);


   glEnd();
   glPopMatrix();
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);



}


GLvoid DungeonConstruction(GLvoid)
{
    GLfloat x_m, y_m, z_m, u_m, v_m;
    GLfloat xtrans, ztrans, ytrans;
    GLfloat sceneroty;
    int totalWalls;
//camera variables
    xtrans = -xpos;
    ztrans = -zpos;
    ytrans = -mov1-0.5;
    sceneroty = 360.0 - yrot;
    	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
    glLoadIdentity();


    glRotatef(lookupdown, 1.0, 0, 0);
    glRotatef(sceneroty, 0, 1.0, 0);

    glTranslatef(xtrans, ytrans, ztrans);    
    glBindTexture(GL_TEXTURE_2D,texture[0]);



    totalWalls = BUILD1.totalWalls;
//Here we are constructing our dungeon from the triangular vertices in the txt file.
    for (i=0; i<totalWalls; i++) {       
	glBegin(GL_TRIANGLES);
	if(BUILD1.triangle[i].vertex[0].x - BUILD1.triangle[i].vertex[2].x != 0)
	{		
	glNormal3f( 0.0, 0.0, 1.0);
	x_m = BUILD1.triangle[i].vertex[0].x;
	y_m = BUILD1.triangle[i].vertex[0].y;
	z_m = BUILD1.triangle[i].vertex[0].z;
	u_m = BUILD1.triangle[i].vertex[0].u;
	v_m = BUILD1.triangle[i].vertex[0].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = BUILD1.triangle[i].vertex[1].x;
	y_m = BUILD1.triangle[i].vertex[1].y;
	z_m = BUILD1.triangle[i].vertex[1].z;
	u_m = BUILD1.triangle[i].vertex[1].u;
	v_m = BUILD1.triangle[i].vertex[1].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = BUILD1.triangle[i].vertex[2].x;
	y_m = BUILD1.triangle[i].vertex[2].y;
	z_m = BUILD1.triangle[i].vertex[2].z;
	u_m = BUILD1.triangle[i].vertex[2].u;
	v_m = BUILD1.triangle[i].vertex[2].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);	
	glEnd();
	}
	else 
	glNormal3f( 1.0, 0.0, 0.0);
	x_m = BUILD1.triangle[i].vertex[0].x;
	y_m = BUILD1.triangle[i].vertex[0].y;
	z_m = BUILD1.triangle[i].vertex[0].z;
	u_m = BUILD1.triangle[i].vertex[0].u;
	v_m = BUILD1.triangle[i].vertex[0].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = BUILD1.triangle[i].vertex[1].x;
	y_m = BUILD1.triangle[i].vertex[1].y;
	z_m = BUILD1.triangle[i].vertex[1].z;
	u_m = BUILD1.triangle[i].vertex[1].u;
	v_m = BUILD1.triangle[i].vertex[1].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);
	
	x_m = BUILD1.triangle[i].vertex[2].x;
	y_m = BUILD1.triangle[i].vertex[2].y;
	z_m = BUILD1.triangle[i].vertex[2].z;
	u_m = BUILD1.triangle[i].vertex[2].u;
	v_m = BUILD1.triangle[i].vertex[2].v;
	glTexCoord2f(u_m,v_m); 
	glVertex3f(x_m,y_m,z_m);	
	
	glEnd();	
    
}
   glDisable(GL_TEXTURE_2D);

//Gems
/*
GLfloat LightDiffuse1[]  = {0.25, 0.25, 0.25, 1}; 
GLfloat LightPosition1[] = {1, 0.5, -2.5, 0};
glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse1);
glLightfv(GL_LIGHT1, GL_POSITION, LightPosition1);
//glEnable(GL_LIGHT1);
*/
gem(LightPosition1[0],LightPosition1[1],LightPosition1[2], 0.25,0.25,0.25,gemSpin, 2);
/*
GLfloat LightDiffuse2[]  = {0.25, 0.25, 0.25, 1}; 
GLfloat LightPosition2[] = {1.25, 0.5, -1, 0};
glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuse2);
glLightfv(GL_LIGHT2, GL_POSITION, LightPosition2);
*/
//glEnable(GL_LIGHT2);
gem(LightPosition2[0],LightPosition2[1],LightPosition2[2], 0.25,0.25,0.25,gemSpin, 2);
/*
GLfloat LightDiffuse3[]  = {0.25, 0.25, 0.25, 1}; 
GLfloat LightPosition3[] = {-1, 0.5, -1, 0};
glLightfv(GL_LIGHT3, GL_DIFFUSE, LightDiffuse3);
glLightfv(GL_LIGHT3, GL_POSITION, LightPosition3);
//glEnable(GL_LIGHT3);
*/
gem(LightPosition3[0],LightPosition3[1],LightPosition3[2], 0.25,0.25,0.25,gemSpin, 2);
/*
GLfloat LightDiffuse4[]  = {0.25, 0.25, 0.25, 1}; 
GLfloat LightPosition4[] = {-5, 0.5, -7, 0};
glLightfv(GL_LIGHT4, GL_DIFFUSE, LightDiffuse4);
glLightfv(GL_LIGHT4, GL_POSITION, LightPosition4);
//glEnable(GL_LIGHT4);
*/
gem(LightPosition4[0],LightPosition4[1],LightPosition4[2], 0.25,0.25,0.25,gemSpin, 2);
/*
GLfloat LightDiffuse5[]  = {0.25, 0.25, 0.25, 1}; 
GLfloat LightPosition5[] = {6, 0.5, -4, 0};
glLightfv(GL_LIGHT5, GL_DIFFUSE, LightDiffuse5);
glLightfv(GL_LIGHT5, GL_POSITION, LightPosition5);
//glEnable(GL_LIGHT5);
*/
gem(LightPosition5[0],LightPosition5[1],LightPosition5[2], 0.25,0.25,0.25,gemSpin, 2);
/*
GLfloat LightDiffuse6[]  = {0.25, 0.25, 0.25, 1}; 
GLfloat LightPosition6[] = {-5, 0.5, 0, 0};
glLightfv(GL_LIGHT6, GL_DIFFUSE, LightDiffuse6);
glLightfv(GL_LIGHT6, GL_POSITION, LightPosition6);
*/
//Brain monster 1
brain(0+2.5,0.5,-1+2, 0.1,0.1,0.1, 0, 8);
ghost(-0.05+2.5,0.5,-0.845+2,0.025, 5);
ghost(0.05+2.5,0.5,-0.845+2,0.025, 5);
ghost(-0.05+2.5,0.5,-0.8225+2,0.005, 4);
ghost(0.05+2.5,0.5,-0.8225+2,0.005, 4);
tentacles(0.05+2.5,0.45,-0.95+2, 0.01,0.09,0.01, fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(-0.05+2.5,0.45,-0.95+2, 0.01,0.09,0.01, -fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(0.05+2.5,0.45,-1.1+2, 0.01,0.05,0.01, fabs(75*Sin(ghostChew*0.1)/2.3), 8);
tentacles(-0.05+2.5,0.45,-1.1+2, 0.01,0.05,0.01, -fabs(75*Sin(ghostChew*0.1))/2.3, 8);

mouthTop(0+2.5,0.425+(0.02*fabs(Sin(ghostChew/10))),-0.875+2, 0.1,0.05,0.1, 0, 6);
mouthBottom(0+2.5,0.425-(0.02*fabs(Sin(ghostChew/10))),-0.875+2, 0.1,0.05,0.1, 0, 6);

//Brain monster 2
brain(5+2,0.5,-3-1.25, 0.1,0.1,0.1, 0, 8);
ghost(4.95+2,0.5,-2.845-1.25,0.025, 5);
ghost(5.05+2,0.5,-2.845-1.25,0.025, 5);
ghost(4.95+2,0.5,-2.8225-1.25,0.005, 4);
ghost(5.05+2,0.5,-2.8225-1.25,0.005, 4);
tentacles(5.05+2,0.45,-2.95-1.25, 0.01,0.09,0.01, fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(4.95+2,0.45,-2.95-1.25, 0.01,0.09,0.01, -fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(5.05+2,0.45,-3.1-1.25, 0.01,0.05,0.01, fabs(75*Sin(ghostChew*0.1)/2.3), 8);
tentacles(4.95+2,0.45,-3.1-1.25, 0.01,0.05,0.01, -fabs(75*Sin(ghostChew*0.1))/2.3, 8);

mouthTop(5+2,0.425+(0.02*fabs(Sin(ghostChew/10))),-2.875-1.25, 0.1,0.05,0.1, 0, 6);
mouthBottom(5+2,0.425-(0.02*fabs(Sin(ghostChew/10))),-2.875-1.25, 0.1,0.05,0.1, 0, 6);

//Brain monster 3
brain(-3+6.5,0.5,-1-6, 0.1,0.1,0.1, 0, 8);
ghost(-3.05+6.5,0.5,-0.845-6,0.025, 5);
ghost(-2.95+6.5,0.5,-0.845-6,0.025, 5);
ghost(-3.05+6.5,0.5,-0.8225-6,0.005, 4);
ghost(-2.95+6.5,0.5,-0.8225-6,0.005, 4);
tentacles(-2.95+6.5,0.45,-0.95-6, 0.01,0.09,0.01, fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(-3.05+6.5,0.45,-0.95-6, 0.01,0.09,0.01, -fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(-2.95+6.5,0.45,-1.1-6, 0.01,0.05,0.01, fabs(75*Sin(ghostChew*0.1)/2.3), 8);
tentacles(-3.05+6.5,0.45,-1.1-6, 0.01,0.05,0.01, -fabs(75*Sin(ghostChew*0.1))/2.3, 8);

mouthTop(-3+6.5,0.425+(0.02*fabs(Sin(ghostChew/10))),-0.875-6, 0.1,0.05,0.1, 0, 6);
mouthBottom(-3+6.5,0.425-(0.02*fabs(Sin(ghostChew/10))),-0.875-6, 0.1,0.05,0.1, 0, 6);


//Brain monster 4
brain(0+7,0.5,-1, 0.1,0.1,0.1, 0, 8);
ghost(-0.05+7,0.5,-0.845,0.025, 5);
ghost(0.05+7,0.5,-0.845,0.025, 5);
ghost(-0.05+7,0.5,-0.8225,0.005, 4);
ghost(0.05+7,0.5,-0.8225,0.005, 4);
tentacles(0.05+7,0.45,-0.95, 0.01,0.09,0.01, fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(-0.05+7,0.45,-0.95, 0.01,0.09,0.01, -fabs(75*Sin(ghostChew*0.1)), 8);
tentacles(0.05+7,0.45,-1.1, 0.01,0.05,0.01, fabs(75*Sin(ghostChew*0.1)/2.3), 8);
tentacles(-0.05+7,0.45,-1.1, 0.01,0.05,0.01, -fabs(75*Sin(ghostChew*0.1))/2.3, 8);

mouthTop(0+7,0.425+(0.02*fabs(Sin(ghostChew/10))),-0.875, 0.1,0.05,0.1, 0, 6);
mouthBottom(0+7,0.425-(0.02*fabs(Sin(ghostChew/10))),-0.875, 0.1,0.05,0.1, 0, 6);



//worm1
ghost(-5.1+11,0.2,0.05+0.04*Sin((ghostChew*0.1)+25)-4,0.025, 5);
ghost(-5.1+11,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25)-4,0.025, 5);
ghost(-5.125+11,0.2,0.05+0.04*Sin((ghostChew*0.1)+25)-4,0.005, 4);
ghost(-5.125+11,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25)-4,0.005, 4);

antennae(-5.1+11,0.525-0.3,0.05+0.04*Sin((ghostChew*0.1)+25)-4, 0.005,0.08,0.005, 45, 9);
antennae(-5.1+11,0.525-0.3,-0.05+0.04*Sin((ghostChew*0.1)+25)-4, 0.005,0.09,0.005, -45, 9);
wormHead(-5.1+11,0.45-0.3,0+0.04*Sin((ghostChew*0.1)+25)-4, 0.1,0.1,0.1, 0, 9);
wormSegment(-5+11,0.5-0.3,0+0.04*Sin(ghostChew*0.1)-4, 0.1,0.1,0.1, 0, 9);
wormSegment(-4.9+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-25)-4, 0.09,0.09,0.09, 0, 9);
wormSegment(-4.8+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-50)-4, 0.08,0.08,0.08, 0, 9);
wormSegment(-4.7+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-75)-4, 0.07,0.07,0.07, 0, 9);
wormSegment(-4.6+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-100)-4, 0.06,0.06,0.06, 0, 9);
wormSegment(-4.5+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-125)-4, 0.05,0.05,0.05, 0, 9);
wormSegment(-4.45+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-150)-4, 0.04,0.04,0.04, 0, 9);
wormSegment(-4.4+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-175)-4, 0.03,0.03,0.03, 0, 9);
wormSegment(-4.35+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-200)-4, 0.02,0.02,0.02, 0, 9);
wormSegment(-4.32+11,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-225)-4, 0.01,0.01,0.01, 0, 9);

//worm2
ghost(-5.1+10,0.2,0.05+0.04*Sin((ghostChew*0.1)+25)+4.5,0.025, 5);
ghost(-5.1+10,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25)+4.5,0.025, 5);
ghost(-5.125+10,0.2,0.05+0.04*Sin((ghostChew*0.1)+25)+4.5,0.005, 4);
ghost(-5.125+10,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25)+4.5,0.005, 4);

antennae(-5.1+10,0.525-0.3,0.05+0.04*Sin((ghostChew*0.1)+25)+4.5, 0.005,0.08,0.005, 45, 9);
antennae(-5.1+10,0.525-0.3,-0.05+0.04*Sin((ghostChew*0.1)+25)+4.5, 0.005,0.09,0.005, -45, 9);
wormHead(-5.1+10,0.45-0.3,0+0.04*Sin((ghostChew*0.1)+25)+4.5, 0.1,0.1,0.1, 0, 9);
wormSegment(-5+10,0.5-0.3,0+0.04*Sin(ghostChew*0.1)+4.5, 0.1,0.1,0.1, 0, 9);
wormSegment(-4.9+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-25)+4.5, 0.09,0.09,0.09, 0, 9);
wormSegment(-4.8+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-50)+4.5, 0.08,0.08,0.08, 0, 9);
wormSegment(-4.7+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-75)+4.5, 0.07,0.07,0.07, 0, 9);
wormSegment(-4.6+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-100)+4.5, 0.06,0.06,0.06, 0, 9);
wormSegment(-4.5+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-125)+4.5, 0.05,0.05,0.05, 0, 9);
wormSegment(-4.45+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-150)+4.5, 0.04,0.04,0.04, 0, 9);
wormSegment(-4.4+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-175)+4.5, 0.03,0.03,0.03, 0, 9);
wormSegment(-4.35+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-200)+4.5, 0.02,0.02,0.02, 0, 9);
wormSegment(-4.32+10,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-225)+4.5, 0.01,0.01,0.01, 0, 9);

//worm3
ghost(-5.1+7,0.2,0.05+0.04*Sin((ghostChew*0.1)+25),0.025, 5);
ghost(-5.1+7,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25),0.025, 5);
ghost(-5.125+7,0.2,0.05+0.04*Sin((ghostChew*0.1)+25),0.005, 4);
ghost(-5.125+7,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25),0.005, 4);

antennae(-5.1+7,0.525-0.3,0.05+0.04*Sin((ghostChew*0.1)+25), 0.005,0.08,0.005, 45, 9);
antennae(-5.1+7,0.525-0.3,-0.05+0.04*Sin((ghostChew*0.1)+25), 0.005,0.09,0.005, -45, 9);
wormHead(-5.1+7,0.45-0.3,0+0.04*Sin((ghostChew*0.1)+25), 0.1,0.1,0.1, 0, 9);
wormSegment(-5+7,0.5-0.3,0+0.04*Sin(ghostChew*0.1), 0.1,0.1,0.1, 0, 9);
wormSegment(-4.9+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-25), 0.09,0.09,0.09, 0, 9);
wormSegment(-4.8+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-50), 0.08,0.08,0.08, 0, 9);
wormSegment(-4.7+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-75), 0.07,0.07,0.07, 0, 9);
wormSegment(-4.6+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-100), 0.06,0.06,0.06, 0, 9);
wormSegment(-4.5+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-125), 0.05,0.05,0.05, 0, 9);
wormSegment(-4.45+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-150), 0.04,0.04,0.04, 0, 9);
wormSegment(-4.4+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-175), 0.03,0.03,0.03, 0, 9);
wormSegment(-4.35+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-200), 0.02,0.02,0.02, 0, 9);
wormSegment(-4.32+7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-225), 0.01,0.01,0.01, 0, 9);
/*
//worm4
ghost(-5.1,0.2,0.05+0.04*Sin((ghostChew*0.1)+25),0.025, 5);
ghost(-5.1,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25),0.025, 5);
ghost(-5.125,0.2,0.05+0.04*Sin((ghostChew*0.1)+25),0.005, 4);
ghost(-5.125,0.2,-0.05+0.04*Sin((ghostChew*0.1)+25),0.005, 4);

antennae(-5.1,0.525-0.3,0.05+0.04*Sin((ghostChew*0.1)+25), 0.005,0.08,0.005, 45, 9);
antennae(-5.1,0.525-0.3,-0.05+0.04*Sin((ghostChew*0.1)+25), 0.005,0.09,0.005, -45, 9);
wormHead(-5.1,0.45-0.3,0+0.04*Sin((ghostChew*0.1)+25), 0.1,0.1,0.1, 0, 9);
wormSegment(-5,0.5-0.3,0+0.04*Sin(ghostChew*0.1), 0.1,0.1,0.1, 0, 9);
wormSegment(-4.9,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-25), 0.09,0.09,0.09, 0, 9);
wormSegment(-4.8,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-50), 0.08,0.08,0.08, 0, 9);
wormSegment(-4.7,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-75), 0.07,0.07,0.07, 0, 9);
wormSegment(-4.6,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-100), 0.06,0.06,0.06, 0, 9);
wormSegment(-4.5,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-125), 0.05,0.05,0.05, 0, 9);
wormSegment(-4.45,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-150), 0.04,0.04,0.04, 0, 9);
wormSegment(-4.4,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-175), 0.03,0.03,0.03, 0, 9);
wormSegment(-4.35,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-200), 0.02,0.02,0.02, 0, 9);
wormSegment(-4.32,0.5-0.3,0+0.04*Sin((ghostChew*0.1)-225), 0.01,0.01,0.01, 0, 9);
*/
//Ceiling

for(j = -10; j <=10; j++)
{
for(k = -10; k <=10; k++)
{
ceilingsFloors(j, 1, k, 1,0,1, 0, 1);
k=k+1;
}
j=j+1;
}
	
//floor

for(j = -10; j <=10; j++)
{
for(k = -10; k <=10; k++)
{
ceilingsFloors(j, 0, k, 1,0,1, 0, 1);
k=k+1;
}
j=j+1;
}
//warps
warpWall(0,0.5,1, 0.15,0.15,0, gemSpin*2, 7);
warpWall(4,0.5,-6, 0.15,0.15,0, gemSpin*2, 7);
warpWall(-7,0.5,-6, 0.15,0.15,0, gemSpin*2, 7);
warpWall(7,0.5,4, 0.15,0.15,0, gemSpin*2, 7);

   glEnd();
    glutSwapBuffers();
}


void keyPressed(unsigned char key, int x, int y) 
{

    switch (key) {    
    case ESCAPE: 
	exit(1);                   	
	break;
	
	case 'l': 
    case 'L': // switch the lighting
	printf("L/l pressed; lighting is: %d\n", light);
	light = light ? 0 : 1;              // switch the current value of light, between 0 and 1.
	if (light) {
	    glEnable(GL_LIGHTING);
	} else {
	    glDisable(GL_LIGHTING);
	}
	printf("Lighting is now: %d\n", light);
	break;

    default:
      printf ("Key %d pressed. No action there yet.\n", key);
      break;
    }	
}

void specialKeyPressed(int key, int x, int y) 
{


    switch (key) {    
    case GLUT_KEY_PAGE_UP: // look up
	z -= 0.2;
	lookupdown -= 0.2;
	break;
    
    case GLUT_KEY_PAGE_DOWN: // look down
	z += 0.2;
	lookupdown += 1.0;
	break;

    case GLUT_KEY_UP: // walk forward
	xpos -= (float)sin(yrot*piover180) * 0.05;
	zpos -= (float)cos(yrot*piover180) * 0.05;
    if (zpos >=-3 && zpos <=1.0 && xpos >= 0.3 && xpos <= 0.5)//collision for right wall of central hallway
    {
		xpos= 0.299;
	}
	if (zpos >=-3 && zpos <=1.0 && xpos >= 0.5 && xpos <= 0.7)//collision for right wall of central hallway (back end)
    {
		xpos= 0.701;
	}


	if (xpos >=0.5 && xpos <=2 && zpos <= -2 && zpos >= -2.2 )//in between wall in corridor
    {
		zpos= -2.201;
	}
	if (xpos >=0.5 && xpos <=2 && zpos >= -2 && zpos <= -1.8 )//in between wall in corridor(back end)
    {
		zpos= -1.799;
	}
	
	
	if (xpos >=0.5 && xpos <=2 && zpos <= -3 && zpos >= -3.2 )//in between wall in corridor
    {
		zpos= -3.201;
	}
	if (xpos >=0.5 && xpos <=2 && zpos >= -3 && zpos <= -2.8 )//in between wall in corridor(back end)
    {
		zpos= -2.799;
	}
	
	
	if (zpos <=-0.5 && zpos >=-3 && xpos <= 2.2 && xpos >= 2 )//wall of left turn from previous
    {
		xpos= 2.201;
	}
	if (zpos <=-0.5 && zpos >=-3 && xpos <= 2 && xpos >= 1.8 )//wall of left turn from previous(back end)
    {
		xpos= 1.799;
	}
	

	if (xpos >=2 && xpos <=3 && zpos >= -0.5 && zpos <= -0.3 )//parallel to previous
    {
		zpos= -0.299;
	}
	if (xpos >=2 && xpos <=3 && zpos <= -0.5 && zpos >= -0.7 )//parallel to previous (back end)
    {
		zpos= -0.701;
	}


	if (zpos <=-0.5 && zpos >=-3.5 && xpos <= 3.2 && xpos >= 3 )//wall of left turn from previous
    {
		xpos= 3.201;
	}
	if (zpos <=-0.5 && zpos >=-3.5 && xpos <= 3 && xpos >= 2.8 )//wall of left turn from previous (back end)
    {
		xpos= 2.799;
	}


    if (zpos >=2 && zpos <=7.0 && xpos >= 0.3 && xpos <= 0.5)//collision for right wall of central hallway
    {
		xpos= 0.299;
	}
    if (zpos >=2 && zpos <=7.0 && xpos >= 0.5 && xpos <= 0.7)//collision for right wall of central hallway (back end)
    {
		xpos= 0.701;
	}


    if (zpos >=-4 && zpos <=0.0 && xpos <= -0.3 && xpos >= -0.5)//collision for left wall of central hallway
    {
		xpos= -0.299;
	}
    if (zpos >=-4 && zpos <=0.0 && xpos <= -0.5 && xpos >= -0.7)//collision for left wall of central hallway (back end)
    {
		xpos= -0.701;
	}

	
   if (zpos >=1 && zpos <=8.0 && xpos <= -0.3 && xpos >= -0.5)//collision for left wall of central hallway
    {
		xpos= -0.299;
	}
   if (zpos >=1 && zpos <=8.0 && xpos <= -0.5 && xpos >= -0.7)//collision for left wall of central hallway (back end)
    {
		xpos= -0.701;
	}

    if (zpos <=-3.8 && zpos >=-4.0 && xpos >= -0.5 && xpos <= 2.5 )//collision for front end of central hallway
    {
		zpos= -3.799;
	}
    if (zpos <=-4 && zpos >=-4.2 && xpos >= -0.5 && xpos <= 2.5 )//collision for front end of central hallway (back end)
    {
		zpos= -4.201;
	}	
	
	if (zpos <=-4 && zpos >=-8.0 && xpos <= 2.7 && xpos >= 2.5 )//wall of left turn from previous
    {
		xpos= 2.701;
	}
	if (zpos <=-4 && zpos >=-8.0 && xpos <= 2.5 && xpos >= 2.3 )//wall of left turn from previous (back end)
    {
		xpos= 2.299;
	}

	
	if (zpos <=-0.3 && zpos >=-0.5 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway left wall
    {
		zpos= -0.299;
	}
	if (zpos <=-0.5 && zpos >=-0.7 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway left wall (back end)
    {
		zpos= -0.701;
	}
	
	
	if (zpos <=-0.5 && zpos >=-2.0 && xpos >= -2.0 && xpos <= -1.8)//slight left turn
    {
		xpos= -1.799;
	}
	if (zpos <=-0.5 && zpos >=-2.0 && xpos <= -2.0 && xpos >= -2.2)//slight left turn (back end)
    {
		xpos= -2.201;
	}
	
	
	if (zpos >=-0.5 && zpos <=0.5 && xpos <= -9.8)//end of starting hallway
    {
		xpos= -9.799;
	}

	if (zpos >=-2 && zpos <=-1.8 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall
    {
		zpos= -1.799;
	}
	if (zpos <=-2 && zpos >=-2.2 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall (back end)
    {
		zpos= -2.201;
	}


	if (zpos >=0.3 && zpos <=0.5 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway right wall
    {
		zpos= 0.299;
	}
	if (zpos >=0.5 && zpos <=0.7 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway right wall (back end)
    {
		zpos= 0.701;
	}
	
	
   if (zpos >=0.5 && zpos <=2.0 && xpos >= -2.0 && xpos <= -1.8)//slight right turn
    {
		xpos= -1.799;
	}
   if (zpos >=0.5 && zpos <=2.0 && xpos <= -2.0 && xpos <= -2.2)//slight right turn (back end)
    {
		xpos= -2.201;
	}


	if (zpos <=2 && zpos >=1.8 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall
    {
		zpos= 1.799;
	}
	if (zpos >=2 && zpos <=2.2 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall (back end)
    {
		zpos= 2.201;
	}

 
    if(zpos <= -7.8) //-Z end wall
    {
            zpos=-7.799;
    }
    if(xpos >= 8.3) //+X end wall
    {
            xpos=8.299;
    }
    
    
    if (zpos <=-4.8 && zpos >=-5.0 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z wall
    {
		zpos= -4.799;
	}
    if (zpos <=-5 && zpos >=-5.2 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z wall (back end)
    {
		zpos= -5.201;
	}


	if (zpos <=-7 && zpos >=-7.2 && xpos >= 4.5 && xpos <= 6.5 )//collision for +X -Z 1st branching wall
    {
		zpos= -7.201;
	}
	if (zpos >=-7 && zpos <=-6.8 && xpos >= 4.5 && xpos <= 6.5 )//collision for +X -Z 1st branching wall (back end)
    {
		zpos= -6.799;
	}
	
	
	if (zpos <=-4.0 && zpos >=-7.0 && xpos >= 4.3 && xpos <= 4.5 )//in between wall
    {
		xpos= 4.299;
	}
	if (zpos <=-4.0 && zpos >=-7.0 && xpos >= 4.5 && xpos <= 4.7 )//in between wall (back end)
    {
		xpos= 4.701;
	}


	if (zpos <=-5 && zpos >=-5.2 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z  2nd branching wall
    {
		zpos= -5.201;
	}
	if (zpos >=-5 && zpos <=-4.8 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z  2nd branching wall (back end)
    {
		zpos= -5.201;
	}


    if (zpos >=4.8 && zpos <=5.0 && xpos <= 8.5 && xpos >= 2.5 )//collision for wall at +Z end of +X wall
    {
		zpos= 4.799;
	}
    if (zpos >=5 && zpos <=5.2 && xpos <= 8.5 && xpos >= 2.5 )//collision for wall at +Z end of +X wall (back end)
    {
		zpos= 5.201;
	}


	if (zpos >=5.0 && zpos <=8.0 && xpos >= 2.3 && xpos <= 2.5 )//wall of left turn from previous
    {
		xpos= 2.299;
	}
	if (zpos >=5.0 && zpos <=8.0 && xpos >= 2.5 && xpos <= 2.7 )//wall of left turn from previous (back end)
    {
		xpos= 2.701;
	}


    if (zpos >=7.8 && zpos <=8.0 && xpos <= 2.5 && xpos >= -0.5 )//collision for +Z Wall
    {
		zpos= 7.799;
	}
    if (zpos >=8 && zpos <=8.2 && xpos <= 2.5 && xpos >= -0.5 )//collision for +Z Wall (back end)
    {
		zpos= 8.201;
	}


    if (zpos >=7.0 && zpos <=7.2 && xpos <= 1.5 && xpos >= 0.5 )//Wall parallel to +Z wall
    {
		zpos= 7.201;
	}
    if (zpos <=7.0 && zpos >=6.8 && xpos <= 1.5 && xpos >= 0.5 )//Wall parallel to +Z wall (back end)
    {
		zpos= 6.799;
	}


	if (zpos >=3.0 && zpos <=7.0 && xpos <= 1.7 && xpos >= 1.5 )//wall of left turn from previous
    {
		xpos= 1.701;
	}
	if (zpos >=3.0 && zpos <=7.0 && xpos <= 1.5 && xpos >= 1.3 )//wall of left turn from previous (back end)
    {
		xpos= 1.299;
	}

	
	if (xpos <=1.5 && xpos >=0.5 && zpos <= 3 && zpos >= 2.8 )//wall of left turn from previous
    {
		zpos= 2.799;
	}
	if (xpos <=1.5 && xpos >=0.5 && zpos >= 3 && zpos <= 3.2 )//wall of left turn from previous (back end)
    {
		zpos= 3.201;
	}

	
	if (xpos <=2 && xpos >=0.5 && zpos >= 2 && zpos <= 2.2 )//parallel to previous
    {
		zpos= 2.201;
	}
	if (xpos <=2 && xpos >=0.5 && zpos <= 2 && zpos >= 1.8 )//parallel to previous (back end)
    {
		zpos= 1.799;
	}


	if (zpos <=2.0 && zpos >=0.5 && xpos <= 2.2 && xpos >= 2 )//wall of left turn from previous
    {
		xpos= 2.201;
	}
	if (zpos <=2.0 && zpos >=0.5 && xpos <= 2 && xpos >= 1.8 )//wall of left turn from previous (back end)
    {
		xpos= 1.799;
	}


	if (xpos >=2 && xpos <=4 && zpos >= 0.5 && zpos <= 0.7 )//parallel to previous
    {
		zpos= 0.701;
	}
	if (xpos >=2 && xpos <=4 && zpos <= 0.5 && zpos >= 0.3 )//parallel to previous (back end)
    {
		zpos= 0.299;
	}


	if (zpos <=0.5 && zpos >=-2 && xpos <= 4.2 && xpos >= 4 )//wall of left turn from previous
    {
		xpos= 4.201;
	}
	if (zpos <=0.5 && zpos >=-2 && xpos <= 4 && xpos >= 3.8 )//wall of left turn from previous (back end)
    {
		xpos= 3.799;
	}


	if (xpos >=4 && xpos <=7.5 && zpos >= -2 && zpos <= -1.8 )//parallel to previous
    {
		zpos= -1.799;
	}
	if (xpos >=4 && xpos <=7.5 && zpos <= -2 && zpos >= -2.2 )//parallel to previous (back end)
    {
		zpos= -2.201;
	}


	if (zpos >=3.0 && zpos <=4.0 && xpos <= 2.5 && xpos >= 2.3 )//wall of left turn from previous
    {
		xpos= 2.299;
	}
	if (zpos >=3.0 && zpos <=4.0 && xpos >= 2.5 && xpos <= 2.7 )//wall of left turn from previous (back end)
    {
		xpos= 2.701;
	}


	if (zpos >=2.8 && zpos <=3 && xpos <= 4.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 2.799;
	}
	if (zpos >=3 && zpos <=3.2 && xpos <= 4.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall (back end)
    {
		zpos= 3.201;
	}


	if (zpos >=4.0 && zpos <=4.2 && xpos <= 5.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 4.201;
	}
	if (zpos <=4.0 && zpos >=3.8 && xpos <= 5.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall (back end)
    {
		zpos= 3.799;
	}


	if (zpos >=0.0 && zpos <=4.0 && xpos <= 5.7 && xpos >= 5.5 )//wall of left turn from previous
    {
		xpos= 5.701;
	}
	if (zpos >=0.0 && zpos <=4.0 && xpos <= 5.5 && xpos >= 5.3 )//wall of left turn from previous (back end)
    {
		xpos= 5.299;
	}


	if (zpos >=-1.0 && zpos <=1.0 && xpos <= 6.5 && xpos >= 6.3 )//wall of left turn from previous
    {
		xpos= 6.299;
	}
	if (zpos >=-1.0 && zpos <=1.0 && xpos >= 6.5 && xpos <= 6.7 )//wall of left turn from previous (back end)
    {
		xpos= 6.701;
	}


	if (zpos >=1.0 && zpos <=1.2 && xpos <= 7.5 && xpos >= 6.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 1.201;
	}
	if (zpos <=1.0 && zpos >=0.8 && xpos <= 7.5 && xpos >= 6.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 0.799;
	}

	if (mov2 >= 359.0)
	    mov2 = 0.0;	
	else 
	    mov2+= 10;
	mov1 = (float)sin(mov2 * piover180)/30.0f;
	break;

    case GLUT_KEY_DOWN: // walk back
	xpos += (float)sin(yrot*piover180) * 0.05;
	zpos += (float)cos(yrot*piover180) * 0.05;	
	    if (zpos >=-3 && zpos <=1.0 && xpos >= 0.3 && xpos <= 0.5)//collision for right wall of central hallway
    {
		xpos= 0.299;
	}
	if (zpos >=-3 && zpos <=1.0 && xpos >= 0.5 && xpos <= 0.7)//collision for right wall of central hallway (back end)
    {
		xpos= 0.701;
	}


	if (xpos >=0.5 && xpos <=2 && zpos <= -2 && zpos >= -2.2 )//in between wall in corridor
    {
		zpos= -2.201;
	}
	if (xpos >=0.5 && xpos <=2 && zpos >= -2 && zpos <= -1.8 )//in between wall in corridor(back end)
    {
		zpos= -1.799;
	}
	
	
	if (xpos >=0.5 && xpos <=2 && zpos <= -3 && zpos >= -3.2 )//in between wall in corridor
    {
		zpos= -3.201;
	}
	if (xpos >=0.5 && xpos <=2 && zpos >= -3 && zpos <= -2.8 )//in between wall in corridor(back end)
    {
		zpos= -2.799;
	}
	
	
	if (zpos <=-0.5 && zpos >=-3 && xpos <= 2.2 && xpos >= 2 )//wall of left turn from previous
    {
		xpos= 2.201;
	}
	if (zpos <=-0.5 && zpos >=-3 && xpos <= 2 && xpos >= 1.8 )//wall of left turn from previous(back end)
    {
		xpos= 1.799;
	}
	

	if (xpos >=2 && xpos <=3 && zpos >= -0.5 && zpos <= -0.3 )//parallel to previous
    {
		zpos= -0.299;
	}
	if (xpos >=2 && xpos <=3 && zpos <= -0.5 && zpos >= -0.7 )//parallel to previous (back end)
    {
		zpos= -0.701;
	}


	if (zpos <=-0.5 && zpos >=-3.5 && xpos <= 3.2 && xpos >= 3 )//wall of left turn from previous
    {
		xpos= 3.201;
	}
	if (zpos <=-0.5 && zpos >=-3.5 && xpos <= 3 && xpos >= 2.8 )//wall of left turn from previous (back end)
    {
		xpos= 2.799;
	}


    if (zpos >=2 && zpos <=7.0 && xpos >= 0.3 && xpos <= 0.5)//collision for right wall of central hallway
    {
		xpos= 0.299;
	}
    if (zpos >=2 && zpos <=7.0 && xpos >= 0.5 && xpos <= 0.7)//collision for right wall of central hallway (back end)
    {
		xpos= 0.701;
	}


    if (zpos >=-4 && zpos <=0.0 && xpos <= -0.3 && xpos >= -0.5)//collision for left wall of central hallway
    {
		xpos= -0.299;
	}
    if (zpos >=-4 && zpos <=0.0 && xpos <= -0.5 && xpos >= -0.7)//collision for left wall of central hallway (back end)
    {
		xpos= -0.701;
	}

	
   if (zpos >=1 && zpos <=8.0 && xpos <= -0.3 && xpos >= -0.5)//collision for left wall of central hallway
    {
		xpos= -0.299;
	}
   if (zpos >=1 && zpos <=8.0 && xpos <= -0.5 && xpos >= -0.7)//collision for left wall of central hallway (back end)
    {
		xpos= -0.701;
	}

    if (zpos <=-3.8 && zpos >=-4.0 && xpos >= -0.5 && xpos <= 2.5 )//collision for front end of central hallway
    {
		zpos= -3.799;
	}
    if (zpos <=-4 && zpos >=-4.2 && xpos >= -0.5 && xpos <= 2.5 )//collision for front end of central hallway (back end)
    {
		zpos= -4.201;
	}	
	
	if (zpos <=-4 && zpos >=-8.0 && xpos <= 2.7 && xpos >= 2.5 )//wall of left turn from previous
    {
		xpos= 2.701;
	}
	if (zpos <=-4 && zpos >=-8.0 && xpos <= 2.5 && xpos >= 2.3 )//wall of left turn from previous (back end)
    {
		xpos= 2.299;
	}

	
	if (zpos <=-0.3 && zpos >=-0.5 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway left wall
    {
		zpos= -0.299;
	}
	if (zpos <=-0.5 && zpos >=-0.7 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway left wall (back end)
    {
		zpos= -0.701;
	}
	
	
	if (zpos <=-0.5 && zpos >=-2.0 && xpos >= -2.0 && xpos <= -1.8)//slight left turn
    {
		xpos= -1.799;
	}
	if (zpos <=-0.5 && zpos >=-2.0 && xpos <= -2.0 && xpos >= -2.2)//slight left turn (back end)
    {
		xpos= -2.201;
	}
	
	
	if (zpos >=-0.5 && zpos <=0.5 && xpos <= -9.8)//end of starting hallway
    {
		xpos= -9.799;
	}

	if (zpos >=-2 && zpos <=-1.8 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall
    {
		zpos= -1.799;
	}
	if (zpos <=-2 && zpos >=-2.2 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall (back end)
    {
		zpos= -2.201;
	}


	if (zpos >=0.3 && zpos <=0.5 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway right wall
    {
		zpos= 0.299;
	}
	if (zpos >=0.5 && zpos <=0.7 && xpos >= -10 && xpos <= -2.0 )//collision for starting (left) hallway right wall (back end)
    {
		zpos= 0.701;
	}
	
	
   if (zpos >=0.5 && zpos <=2.0 && xpos >= -2.0 && xpos <= -1.8)//slight right turn
    {
		xpos= -1.799;
	}
   if (zpos >=0.5 && zpos <=2.0 && xpos <= -2.0 && xpos <= -2.2)//slight right turn (back end)
    {
		xpos= -2.201;
	}


	if (zpos <=2 && zpos >=1.8 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall
    {
		zpos= 1.799;
	}
	if (zpos >=2 && zpos <=2.2 && xpos >= -2 && xpos <= -0.5 )//collision for starting (left) hallway right wall (back end)
    {
		zpos= 2.201;
	}

 
    if(zpos <= -7.8) //-Z end wall
    {
            zpos=-7.799;
    }
    if(xpos >= 8.3) //+X end wall
    {
            xpos=8.299;
    }
    
    
    if (zpos <=-4.8 && zpos >=-5.0 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z wall
    {
		zpos= -4.799;
	}
    if (zpos <=-5 && zpos >=-5.2 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z wall (back end)
    {
		zpos= -5.201;
	}


	if (zpos <=-7 && zpos >=-7.2 && xpos >= 4.5 && xpos <= 6.5 )//collision for +X -Z 1st branching wall
    {
		zpos= -7.201;
	}
	if (zpos >=-7 && zpos <=-6.8 && xpos >= 4.5 && xpos <= 6.5 )//collision for +X -Z 1st branching wall (back end)
    {
		zpos= -6.799;
	}
	
	
	if (zpos <=-4.0 && zpos >=-7.0 && xpos >= 4.3 && xpos <= 4.5 )//in between wall
    {
		xpos= 4.299;
	}
	if (zpos <=-4.0 && zpos >=-7.0 && xpos >= 4.5 && xpos <= 4.7 )//in between wall (back end)
    {
		xpos= 4.701;
	}


	if (zpos <=-5 && zpos >=-5.2 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z  2nd branching wall
    {
		zpos= -5.201;
	}
	if (zpos >=-5 && zpos <=-4.8 && xpos >= 4.5 && xpos <= 8.5 )//collision for +X -Z  2nd branching wall (back end)
    {
		zpos= -5.201;
	}


    if (zpos >=4.8 && zpos <=5.0 && xpos <= 8.5 && xpos >= 2.5 )//collision for wall at +Z end of +X wall
    {
		zpos= 4.799;
	}
    if (zpos >=5 && zpos <=5.2 && xpos <= 8.5 && xpos >= 2.5 )//collision for wall at +Z end of +X wall (back end)
    {
		zpos= 5.201;
	}


	if (zpos >=5.0 && zpos <=8.0 && xpos >= 2.3 && xpos <= 2.5 )//wall of left turn from previous
    {
		xpos= 2.299;
	}
	if (zpos >=5.0 && zpos <=8.0 && xpos >= 2.5 && xpos <= 2.7 )//wall of left turn from previous (back end)
    {
		xpos= 2.701;
	}


    if (zpos >=7.8 && zpos <=8.0 && xpos <= 2.5 && xpos >= -0.5 )//collision for +Z Wall
    {
		zpos= 7.799;
	}
    if (zpos >=8 && zpos <=8.2 && xpos <= 2.5 && xpos >= -0.5 )//collision for +Z Wall (back end)
    {
		zpos= 8.201;
	}


    if (zpos >=7.0 && zpos <=7.2 && xpos <= 1.5 && xpos >= 0.5 )//Wall parallel to +Z wall
    {
		zpos= 7.201;
	}
    if (zpos <=7.0 && zpos >=6.8 && xpos <= 1.5 && xpos >= 0.5 )//Wall parallel to +Z wall (back end)
    {
		zpos= 6.799;
	}


	if (zpos >=3.0 && zpos <=7.0 && xpos <= 1.7 && xpos >= 1.5 )//wall of left turn from previous
    {
		xpos= 1.701;
	}
	if (zpos >=3.0 && zpos <=7.0 && xpos <= 1.5 && xpos >= 1.3 )//wall of left turn from previous (back end)
    {
		xpos= 1.299;
	}

	
	if (xpos <=1.5 && xpos >=0.5 && zpos <= 3 && zpos >= 2.8 )//wall of left turn from previous
    {
		zpos= 2.799;
	}
	if (xpos <=1.5 && xpos >=0.5 && zpos >= 3 && zpos <= 3.2 )//wall of left turn from previous (back end)
    {
		zpos= 3.201;
	}

	
	if (xpos <=2 && xpos >=0.5 && zpos >= 2 && zpos <= 2.2 )//parallel to previous
    {
		zpos= 2.201;
	}
	if (xpos <=2 && xpos >=0.5 && zpos <= 2 && zpos >= 1.8 )//parallel to previous (back end)
    {
		zpos= 1.799;
	}


	if (zpos <=2.0 && zpos >=0.5 && xpos <= 2.2 && xpos >= 2 )//wall of left turn from previous
    {
		xpos= 2.201;
	}
	if (zpos <=2.0 && zpos >=0.5 && xpos <= 2 && xpos >= 1.8 )//wall of left turn from previous (back end)
    {
		xpos= 1.799;
	}


	if (xpos >=2 && xpos <=4 && zpos >= 0.5 && zpos <= 0.7 )//parallel to previous
    {
		zpos= 0.701;
	}
	if (xpos >=2 && xpos <=4 && zpos <= 0.5 && zpos >= 0.3 )//parallel to previous (back end)
    {
		zpos= 0.299;
	}


	if (zpos <=0.5 && zpos >=-2 && xpos <= 4.2 && xpos >= 4 )//wall of left turn from previous
    {
		xpos= 4.201;
	}
	if (zpos <=0.5 && zpos >=-2 && xpos <= 4 && xpos >= 3.8 )//wall of left turn from previous (back end)
    {
		xpos= 3.799;
	}


	if (xpos >=4 && xpos <=7.5 && zpos >= -2 && zpos <= -1.8 )//parallel to previous
    {
		zpos= -1.799;
	}
	if (xpos >=4 && xpos <=7.5 && zpos <= -2 && zpos >= -2.2 )//parallel to previous (back end)
    {
		zpos= -2.201;
	}


	if (zpos >=3.0 && zpos <=4.0 && xpos <= 2.5 && xpos >= 2.3 )//wall of left turn from previous
    {
		xpos= 2.299;
	}
	if (zpos >=3.0 && zpos <=4.0 && xpos >= 2.5 && xpos <= 2.7 )//wall of left turn from previous (back end)
    {
		xpos= 2.701;
	}


	if (zpos >=2.8 && zpos <=3 && xpos <= 4.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 2.799;
	}
	if (zpos >=3 && zpos <=3.2 && xpos <= 4.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall (back end)
    {
		zpos= 3.201;
	}


	if (zpos >=4.0 && zpos <=4.2 && xpos <= 5.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 4.201;
	}
	if (zpos <=4.0 && zpos >=3.8 && xpos <= 5.5 && xpos >= 2.5 )//collision for wall parallel to +Z end of +X wall (back end)
    {
		zpos= 3.799;
	}


	if (zpos >=0.0 && zpos <=4.0 && xpos <= 5.7 && xpos >= 5.5 )//wall of left turn from previous
    {
		xpos= 5.701;
	}
	if (zpos >=0.0 && zpos <=4.0 && xpos <= 5.5 && xpos >= 5.3 )//wall of left turn from previous (back end)
    {
		xpos= 5.299;
	}


	if (zpos >=-1.0 && zpos <=1.0 && xpos <= 6.5 && xpos >= 6.3 )//wall of left turn from previous
    {
		xpos= 6.299;
	}
	if (zpos >=-1.0 && zpos <=1.0 && xpos >= 6.5 && xpos <= 6.7 )//wall of left turn from previous (back end)
    {
		xpos= 6.701;
	}


	if (zpos >=1.0 && zpos <=1.2 && xpos <= 7.5 && xpos >= 6.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 1.201;
	}
	if (zpos <=1.0 && zpos >=0.8 && xpos <= 7.5 && xpos >= 6.5 )//collision for wall parallel to +Z end of +X wall
    {
		zpos= 0.799;
	}

	if (mov2 <= 1.0)
	    mov2 = 359.0;	
	else 
	    mov2-= 10;
	mov1 = (float)sin(mov2 * piover180)/30.0;
	break;

    case GLUT_KEY_LEFT: // look left
	yrot += 1.5;
	break;
    
    case GLUT_KEY_RIGHT: // look right
	yrot -= 1.5;
	break;

	break;
    }	
}

int main(int argc, char **argv) 
{  
    buildMyDungeon(); 
    glutInit(&argc, argv);  
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);  
    glutInitWindowSize(640, 480);  
    glutInitWindowPosition(0, 0);  
    window = glutCreateWindow("Try not to die!!");  
    glutFullScreen();
    glutIdleFunc(idle); 
    glutReshapeFunc(&ReSizeGLScene);
    glutKeyboardFunc(&keyPressed);
    glutSpecialFunc(&specialKeyPressed);
    GLStuff(640, 480);
    glutDisplayFunc(&DungeonConstruction);  

    glutMainLoop();  

    return 1;
}
