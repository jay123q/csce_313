#include <iostream>
#include <cstring>
using namespace std;

struct Point {
    int x, y;

    Point () : x(), y() {}
    Point (int _x, int _y) : x(_x), y(_y) {}
};

class Shape {

    public:
    double * area;
    int vertices;
    Point** points;
    Shape (int _vertices) {
        vertices = _vertices;
        points = new Point*[vertices+1];
    }

    ~Shape () 
    {
        for (size_t i = 0; i < vertices ; i++)
        {
            delete points[i];
        }
        
        delete [] points;
        delete area;
        
    }
/* formal parameter for unsized array called pts */
    void addPoints ( Point * pts) {
        
        for (int i = 0; i < vertices ; i++) {
            points[i] = new Point();
            memcpy(points[i], &pts[i%vertices], sizeof(Point));
        }
    }

    double* area3 () {

        int temp = 0;
        for (int i = 0; i < vertices-1; i++) {
            // FIXME: there are two methods to access members of pointers
            //        use one to fix lhs and the other to fix rhs
            int lhs = points[i]->x * points[i+1]->y;
            int rhs = points[i+1]->x * points[i]->y;
            temp += (lhs - rhs);
        }
        area = new double ( abs(temp)/2.0 ) ;
        return area;
    }
};

int main () {
    // FIXME: create the following points using the three different methods
    //        of defining structs:
    //          tri1 = (0, 0)
    //          tri2 = (1, 2)
    //          tri3 = (2, 0)

    // adding points to tri

    cout << "spot 1 \n";
    Point tri1;
    tri1.x = 0;
    tri1.y = 0;
    cout << "spot 2 \n";

    Point tri2{1,2};
    Point tri3{2,0};
    Point triPts[3] = {tri1, tri2, tri3};


    Shape* tri = new Shape(3);

    tri->addPoints( triPts );

    // FIXME: create the following points using your preferred struct
    //        definition:
    //          quad1 = (0, 0)
    //          quad2 = (0, 2)
    //          quad3 = (2, 2)
    //          quad4 = (2, 0)

    // adding points to quad
    Point quad1; quad1.x =0; quad1.y = 0;
    Point quad2; quad2.x =0; quad2.y = 2;
    Point quad3; quad3.x =2; quad3.y = 2;
    Point quad4; quad4.x =2; quad4.y = 0;
    Point quadPts[4] = {quad1, quad2, quad3, quad4};
    Shape * quad = new Shape(4);

    quad->addPoints( quadPts );
    cout << "spot 3 \n";
    // FIXME: print out area of tri and area of quad
    tri->area3();
    quad->area3();
    std::cout<< " area of triangle " << *tri->area;
    std::cout<< " area of rectangle " << *quad->area;


    delete  tri;
    delete  quad;

}
