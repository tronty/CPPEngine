using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Diagnostics;
using System.Windows.Media.Media3D;

namespace howto_wpf_3d_platonic_icosa_dodeca
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
        }

        // The main object model group.
        private Model3DGroup MainModelGroup = new Model3DGroup();

        // Lights.
        private List<Light> Lights = new List<Light>();

        // The solid and skeleton models, and the axes model.
        private GeometryModel3D AxesModel,
            DedecaFaceModel, DedecaEdgeModel,
            IcosaFaceModel, IcosaEdgeModel;

        // Radii.
        private double
            DodecaInnerRadius, DodecaOuterRadius,
            IcosaInnerRadius, IcosaOuterRadius;

        // The camera.
        private PerspectiveCamera TheCamera;

        // The camera's current location.
        private double CameraPhi = Math.PI / 6.0 - 0 * CameraDPhi;
        private double CameraTheta = Math.PI / 6.0 + 1.5 * CameraDTheta;
        private double CameraR = 4.0;

        // The change in CameraPhi when you press the up and down arrows.
        private const double CameraDPhi = 0.1;

        // The change in CameraTheta when you press the left and right arrows.
        private const double CameraDTheta = 0.1;

        // The change in CameraR when you press + or -.
        private const double CameraDR = 0.1;

        // Create the scene.
        // MainViewport is the Viewport3D defined
        // in the XAML code that displays everything.
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Give the camera its initial position.
            TheCamera = new PerspectiveCamera();
            TheCamera.FieldOfView = 60;
            MainViewport.Camera = TheCamera;
            PositionCamera();

            // Define lights.
            DefineLights();

            // Define the models.
            DefineModels(out AxesModel,
                out DedecaFaceModel, out DedecaEdgeModel,
                out IcosaFaceModel, out IcosaEdgeModel);
            MainModelGroup.Children.Add(AxesModel);
            MainModelGroup.Children.Add(IcosaFaceModel);
            MainModelGroup.Children.Add(DedecaFaceModel);
            MainModelGroup.Children.Add(IcosaEdgeModel);
            MainModelGroup.Children.Add(DedecaEdgeModel);

            // Add the group of models to a ModelVisual3D.
            ModelVisual3D model_visual = new ModelVisual3D();
            model_visual.Content = MainModelGroup;

            // Display the main visual to the viewportt.
            MainViewport.Children.Add(model_visual);
        }

        // Define the lights.
        private void DefineLights()
        {
            Color color64 = Color.FromArgb(255, 128, 128, 64);
            Color color128 = Color.FromArgb(255, 255, 255, 128);
            Lights.Add(new AmbientLight(color64));
            Lights.Add(new DirectionalLight(color128,
                new Vector3D(-1.0, -2.0, -3.0)));
            Lights.Add(new DirectionalLight(color128,
                new Vector3D(1.0, 2.0, 3.0)));

            foreach (Light light in Lights)
                MainModelGroup.Children.Add(light);
        }

        // Return the vertices for an dodecahedron.
        private Point3D[] MakeDodecaVertices(double side_length)
        {
            // Value t1 is actually never used.
            double s = side_length;
            //double t1 = 2.0 * Math.PI / 5.0;
            double t2 = Math.PI / 10.0;
            double t3 = 3.0 * Math.PI / 10.0;
            double t4 = Math.PI / 5.0;
            double d1 = s / 2.0 / Math.Sin(t4);
            double d2 = d1 * Math.Cos(t4);
            double d3 = d1 * Math.Cos(t2);
            double d4 = d1 * Math.Sin(t2);
            double Fx =
                (s * s - (2.0 * d3) * (2.0 * d3) - (d1 * d1 - d3 * d3 - d4 * d4)) /
                (2.0 * (d4 - d1));
            double d5 = Math.Sqrt(0.5 *
                (s * s + (2.0 * d3) * (2.0 * d3) -
                    (d1 - Fx) * (d1 - Fx) -
                        (d4 - Fx) * (d4 - Fx) - d3 * d3));
            double Fy = (Fx * Fx - d1 * d1 - d5 * d5) / (2.0 * d5);
            double Ay = d5 + Fy;

            Point3D A = new Point3D(d1, Ay, 0);
            Point3D B = new Point3D(d4, Ay, d3);
            Point3D C = new Point3D(-d2, Ay, s / 2);
            Point3D D = new Point3D(-d2, Ay, -s / 2);
            Point3D E = new Point3D(d4, Ay, -d3);
            Point3D F = new Point3D(Fx, Fy, 0);
            Point3D G = new Point3D(Fx * Math.Sin(t2), Fy, Fx * Math.Cos(t2));
            Point3D H = new Point3D(-Fx * Math.Sin(t3), Fy, Fx * Math.Cos(t3));
            Point3D I = new Point3D(-Fx * Math.Sin(t3), Fy, -Fx * Math.Cos(t3));
            Point3D J = new Point3D(Fx * Math.Sin(t2), Fy, -Fx * Math.Cos(t2));
            Point3D K = new Point3D(Fx * Math.Sin(t3), -Fy, Fx * Math.Cos(t3));
            Point3D L = new Point3D(-Fx * Math.Sin(t2), -Fy, Fx * Math.Cos(t2));
            Point3D M = new Point3D(-Fx, -Fy, 0);
            Point3D N = new Point3D(-Fx * Math.Sin(t2), -Fy, -Fx * Math.Cos(t2));
            Point3D O = new Point3D(Fx * Math.Sin(t3), -Fy, -Fx * Math.Cos(t3));
            Point3D P = new Point3D(d2, -Ay, s / 2);
            Point3D Q = new Point3D(-d4, -Ay, d3);
            Point3D R = new Point3D(-d1, -Ay, 0);
            Point3D S = new Point3D(-d4, -Ay, -d3);
            Point3D T = new Point3D(d2, -Ay, -s / 2);

            List<Point3D> points = new List<Point3D>();
            points.Add(A);
            points.Add(B);
            points.Add(C);
            points.Add(D);
            points.Add(E);
            points.Add(F);
            points.Add(G);
            points.Add(H);
            points.Add(I);
            points.Add(J);
            points.Add(K);
            points.Add(L);
            points.Add(M);
            points.Add(N);
            points.Add(O);
            points.Add(P);
            points.Add(Q);
            points.Add(R);
            points.Add(S);
            points.Add(T);

            // Calculate dodecahedron radii.
            DodecaInnerRadius = Ay;
            DodecaOuterRadius = Math.Sqrt(d1 * d1 + Ay * Ay);

            return points.ToArray();
        }

        // Return the vertices for an icosahedron.
        private Point3D[] MakeIcosaVertices(double side_length)
        {
            // t1 and t3 are actually not used in calculations.
            double S = side_length;
            //double t1 = 2.0 * Math.PI / 5;
            double t2 = Math.PI / 10.0;
            double t4 = Math.PI / 5.0;
            //double t3 = -3.0 * Math.PI / 10.0;
            double R = (S / 2.0) / Math.Sin(t4);
            double H = Math.Cos(t4) * R;
            double Cx = R * Math.Sin(t2);
            double Cz = R * Math.Cos(t2);
            double H1 = Math.Sqrt(S * S - R * R);
            double H2 = Math.Sqrt((H + R) * (H + R) - H * H);
            double Y2 = (H2 - H1) / 2.0;
            double Y1 = Y2 + H1;

            List<Point3D> points = new List<Point3D>();
            points.Add(new Point3D(0, Y1, 0));
            points.Add(new Point3D(R, Y2, 0));
            points.Add(new Point3D(Cx, Y2, Cz));
            points.Add(new Point3D(-H, Y2, S / 2));
            points.Add(new Point3D(-H, Y2, -S / 2));
            points.Add(new Point3D(Cx, Y2, -Cz));
            points.Add(new Point3D(-R, -Y2, 0));
            points.Add(new Point3D(-Cx, -Y2, -Cz));
            points.Add(new Point3D(H, -Y2, -S / 2));
            points.Add(new Point3D(H, -Y2, S / 2));
            points.Add(new Point3D(-Cx, -Y2, Cz));
            points.Add(new Point3D(0, -Y1, 0));

            // Calculate icosahedron radii.
            double A = (R + Cx) / 3.0;
            double B = (Y1 + 2 * Y2) / 3.0;
            double C = Cz / 3.0;
            IcosaInnerRadius = Math.Sqrt(A * A + B * B + C * C);
            IcosaOuterRadius = Y1;

            return points.ToArray();
        }

        // Create the solid and skeleton models.
        private void DefineModels(
            out GeometryModel3D axes_model,
            out GeometryModel3D dodeca_face_model,
            out GeometryModel3D dodeca_edge_model,
            out GeometryModel3D icosa_face_model,
            out GeometryModel3D icosa_edge_model)
        {
            // Make the axes model.
            MeshGeometry3D axes_mesh = new MeshGeometry3D();
            Point3D origin = new Point3D(0, 0, 0);
            Point3D xmax = new Point3D(1.75, 0, 0);
            Point3D ymax = new Point3D(0, 1.75, 0);
            Point3D zmax = new Point3D(0, 0, 1.75);
            AddSegment(axes_mesh, origin, xmax, new Vector3D(0, 1, 0));
            AddSegment(axes_mesh, origin, zmax, new Vector3D(0, 1, 0));
            AddSegment(axes_mesh, origin, ymax, new Vector3D(1, 0, 0));

            // Add tick marks if desired.
            //Vector3D vx = new Vector3D(0.03, 0, 0);
            //Vector3D vy = new Vector3D(0, 0.03, 0);
            //Vector3D vz = new Vector3D(0, 0, 0.03);
            //for (double x = 0.5; x <= xmax.X; x += 0.5)
            //    AddBox(axes_mesh, new Point3D(x, 0, 0), vx, vy, vz);
            //for (double y = 0.5; y <= ymax.Y; y += 0.5)
            //    AddBox(axes_mesh, new Point3D(0, y, 0), vx, vy, vz);
            //for (double z = 0.5; z <= zmax.Z; z += 0.5)
            //    AddBox(axes_mesh, new Point3D(0, 0, z), vx, vy, vz);

            SolidColorBrush axes_brush = Brushes.Red;
            DiffuseMaterial axes_material = new DiffuseMaterial(axes_brush);
            axes_model = new GeometryModel3D(axes_mesh, axes_material);

            // Get the dodecahedron's points.
            Point3D[] dodeca_points = MakeDodecaVertices(1);

            // Get the icosahedron's points.
            Point3D[] icosa_points = MakeIcosaVertices(1);




            // Scale and rotate the icosahedron's
            // vertices before making the models.

            //// Rotate the icosahedron 180 degrees around the Y axis.
            //Matrix3D transform = Matrix3D.Identity;
            //Quaternion quaternion = new Quaternion(
            //    new Vector3D(0,1,0), 180);
            //transform.RotateAt(quaternion, new Point3D(0, 0, 0));

            //// Scale the icosahedron to give it the
            //// same inner radius as the dodecahedron.
            //double icosa_scale = DodecaInnerRadius / IcosaInnerRadius;
            //transform.ScaleAt(
            //    new Vector3D(icosa_scale, icosa_scale, icosa_scale),
            //    new Point3D(0, 0, 0));

            //// Transform the points.
            //transform.Transform(icosa_points);



            // Create the solid dodecahedron.
            MeshGeometry3D dodeca_face_mesh = new MeshGeometry3D();
            AddPolygon(dodeca_face_mesh, "EDCBA", dodeca_points);
            AddPolygon(dodeca_face_mesh, "ABGKF", dodeca_points);
            AddPolygon(dodeca_face_mesh, "AFOJE", dodeca_points);
            AddPolygon(dodeca_face_mesh, "EJNID", dodeca_points);
            AddPolygon(dodeca_face_mesh, "DIMHC", dodeca_points);
            AddPolygon(dodeca_face_mesh, "CHLGB", dodeca_points);
            AddPolygon(dodeca_face_mesh, "KPTOF", dodeca_points);
            AddPolygon(dodeca_face_mesh, "OTSNJ", dodeca_points);
            AddPolygon(dodeca_face_mesh, "NSRMI", dodeca_points);
            AddPolygon(dodeca_face_mesh, "MRQLH", dodeca_points);
            AddPolygon(dodeca_face_mesh, "LQPKG", dodeca_points);
            AddPolygon(dodeca_face_mesh, "PQRST", dodeca_points);

            SolidColorBrush dodeca_face_brush = Brushes.Orange;
            DiffuseMaterial dodeca_face_material = new DiffuseMaterial(dodeca_face_brush);
            dodeca_face_model = new GeometryModel3D(dodeca_face_mesh, dodeca_face_material);

            // Create the skeleton.
            MeshGeometry3D dodeca_edge_mesh = new MeshGeometry3D();
            Vector3D upy = new Vector3D(0, 1, 0);
            AddPolygonSegments(dodeca_edge_mesh, "EDCBA", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "ABGKF", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "AFOJE", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "EJNID", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "DIMHC", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "CHLGB", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "KPTOF", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "OTSNJ", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "NSRMI", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "MRQLH", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "LQPKG", dodeca_points, upy);
            AddPolygonSegments(dodeca_edge_mesh, "PQRST", dodeca_points, upy);

            SolidColorBrush dodeca_edge_brush = Brushes.Brown;
            DiffuseMaterial dodeca_edge_material = new DiffuseMaterial(dodeca_edge_brush);
            dodeca_edge_model = new GeometryModel3D(dodeca_edge_mesh, dodeca_edge_material);



            // Create the solid icosahedron.
            MeshGeometry3D icosa_face_mesh = new MeshGeometry3D();
            AddTriangle(icosa_face_mesh, icosa_points[0], icosa_points[2], icosa_points[1]);
            AddTriangle(icosa_face_mesh, icosa_points[0], icosa_points[3], icosa_points[2]);
            AddTriangle(icosa_face_mesh, icosa_points[0], icosa_points[4], icosa_points[3]);
            AddTriangle(icosa_face_mesh, icosa_points[0], icosa_points[5], icosa_points[4]);
            AddTriangle(icosa_face_mesh, icosa_points[0], icosa_points[1], icosa_points[5]);

            AddTriangle(icosa_face_mesh, icosa_points[1], icosa_points[2], icosa_points[9]);
            AddTriangle(icosa_face_mesh, icosa_points[2], icosa_points[3], icosa_points[10]);
            AddTriangle(icosa_face_mesh, icosa_points[3], icosa_points[4], icosa_points[6]);
            AddTriangle(icosa_face_mesh, icosa_points[4], icosa_points[5], icosa_points[7]);
            AddTriangle(icosa_face_mesh, icosa_points[5], icosa_points[1], icosa_points[8]);

            AddTriangle(icosa_face_mesh, icosa_points[6], icosa_points[4], icosa_points[7]);
            AddTriangle(icosa_face_mesh, icosa_points[7], icosa_points[5], icosa_points[8]);
            AddTriangle(icosa_face_mesh, icosa_points[8], icosa_points[1], icosa_points[9]);
            AddTriangle(icosa_face_mesh, icosa_points[9], icosa_points[2], icosa_points[10]);
            AddTriangle(icosa_face_mesh, icosa_points[10], icosa_points[3], icosa_points[6]);

            AddTriangle(icosa_face_mesh, icosa_points[11], icosa_points[6], icosa_points[7]);
            AddTriangle(icosa_face_mesh, icosa_points[11], icosa_points[7], icosa_points[8]);
            AddTriangle(icosa_face_mesh, icosa_points[11], icosa_points[8], icosa_points[9]);
            AddTriangle(icosa_face_mesh, icosa_points[11], icosa_points[9], icosa_points[10]);
            AddTriangle(icosa_face_mesh, icosa_points[11], icosa_points[10], icosa_points[6]);

            SolidColorBrush icosa_face_brush = Brushes.LightYellow;
            DiffuseMaterial icosa_face_material = new DiffuseMaterial(icosa_face_brush);
            icosa_face_model = new GeometryModel3D(icosa_face_mesh, icosa_face_material);

            // Create the skeleton.
            MeshGeometry3D icosa_edge_mesh = new MeshGeometry3D();
            for (int i = 1; i < 6; i++)
                AddSegment(icosa_edge_mesh, icosa_points[0], icosa_points[i], upy);
            for (int i = 1; i < 6; i++)
            {
                int pt2 = i + 1;
                if (pt2 > 5) pt2 = 1;
                AddSegment(icosa_edge_mesh, icosa_points[i], icosa_points[pt2], upy);
            }
            AddSegment(icosa_edge_mesh, icosa_points[1], icosa_points[8], upy);
            AddSegment(icosa_edge_mesh, icosa_points[1], icosa_points[9], upy);
            AddSegment(icosa_edge_mesh, icosa_points[2], icosa_points[9], upy);
            AddSegment(icosa_edge_mesh, icosa_points[2], icosa_points[10], upy);
            AddSegment(icosa_edge_mesh, icosa_points[3], icosa_points[10], upy);
            AddSegment(icosa_edge_mesh, icosa_points[3], icosa_points[6], upy);
            AddSegment(icosa_edge_mesh, icosa_points[4], icosa_points[6], upy);
            AddSegment(icosa_edge_mesh, icosa_points[4], icosa_points[7], upy);
            AddSegment(icosa_edge_mesh, icosa_points[5], icosa_points[7], upy);
            AddSegment(icosa_edge_mesh, icosa_points[5], icosa_points[8], upy);
            for (int i = 6; i < 11; i++)
            {
                int pt2 = i + 1;
                if (pt2 > 10) pt2 = 6;
                AddSegment(icosa_edge_mesh, icosa_points[i], icosa_points[pt2], upy);
            }
            for (int i = 6; i < 11; i++)
                AddSegment(icosa_edge_mesh, icosa_points[11], icosa_points[i], upy);

            SolidColorBrush icosa_edge_brush = Brushes.Goldenrod;
            DiffuseMaterial icosa_edge_material = new DiffuseMaterial(icosa_edge_brush);
            icosa_edge_model = new GeometryModel3D(icosa_edge_mesh, icosa_edge_material);



            // Scale and rotate the icosahedron's face and edge models.

            // Rotate the icosahedron 180 degrees around the Y axis.
            Rotation3D rotation = new AxisAngleRotation3D(
                new Vector3D(0, 1, 0), 180);
            RotateTransform3D rotate_transform =
                new RotateTransform3D(rotation, 0, 0, 0);

            // Scale the icosahedron to give it the
            // same inner radius as the dodecahedron.
            double icosa_scale = DodecaInnerRadius / IcosaInnerRadius;
            ScaleTransform3D scale_transform =
                new ScaleTransform3D(
                    icosa_scale, icosa_scale, icosa_scale,
                    0, 0, 0);

            // Transform the models.
            Transform3DGroup transform_group = new Transform3DGroup();
            transform_group.Children.Add(rotate_transform);
            transform_group.Children.Add(scale_transform);
            icosa_face_model.Transform = transform_group;
            icosa_edge_model.Transform = transform_group;
        }

        // Add a polygon by using the point names A, B, C, etc.
        private void AddPolygon(MeshGeometry3D mesh, string point_names, Point3D[] points)
        {
            Point3D[] polygon_points = new Point3D[point_names.Length];
            for (int i = 0; i < point_names.Length; i++)
            {
                polygon_points[i] = points[ToIndex(point_names[i])];
            }
            AddPolygon(mesh, polygon_points);
        }

        // Find a point's index from its letter.
        private int ToIndex(char ch)
        {
            return ch - 'A';
        }

        // Add a polygon to the indicated mesh.
        // Do not reuse old points but reuse these points.
        private void AddPolygon(MeshGeometry3D mesh, params Point3D[] points)
        {
            // Create the points.
            int index1 = mesh.Positions.Count;
            foreach (Point3D point in points)
                mesh.Positions.Add(point);

            // Create the triangles.
            for (int i = 1; i < points.Length - 1; i++)
            {
                mesh.TriangleIndices.Add(index1);
                mesh.TriangleIndices.Add(index1 + i);
                mesh.TriangleIndices.Add(index1 + i + 1);
            }
        }

        // Add a rectangle to the indicated mesh.
        // Do not reuse existing points but reuse these points
        // so new rectangles don't share normals with old ones.
        private void AddRectangle(MeshGeometry3D mesh,
            Point3D point1, Point3D point2, Point3D point3, Point3D point4)
        {
            // Create the points.
            int index1 = mesh.Positions.Count;
            mesh.Positions.Add(point1);
            mesh.Positions.Add(point2);
            mesh.Positions.Add(point3);
            mesh.Positions.Add(point4);

            // Create the triangles.
            mesh.TriangleIndices.Add(index1);
            mesh.TriangleIndices.Add(index1 + 1);
            mesh.TriangleIndices.Add(index1 + 2);

            mesh.TriangleIndices.Add(index1);
            mesh.TriangleIndices.Add(index1 + 2);
            mesh.TriangleIndices.Add(index1 + 3);
        }

        // Add a triangle to the indicated mesh.
        // Do not reuse points so triangles don't share normals.
        private void AddTriangle(MeshGeometry3D mesh, Point3D point1, Point3D point2, Point3D point3)
        {
            // Create the points.
            int index1 = mesh.Positions.Count;
            mesh.Positions.Add(point1);
            mesh.Positions.Add(point2);
            mesh.Positions.Add(point3);

            // Create the triangle.
            mesh.TriangleIndices.Add(index1++);
            mesh.TriangleIndices.Add(index1++);
            mesh.TriangleIndices.Add(index1);
        }

        private void AddPolygonSegments(MeshGeometry3D mesh,
            string point_names, Point3D[] points, Vector3D up)
        {
            for (int i = 0; i < point_names.Length; i++)
            {
                char ch1 = point_names[i];
                char ch2 = point_names[(i + 1) % point_names.Length];
                AddSegment(mesh, points, ch1, ch2, up);
            }
        }
        private void AddSegment(MeshGeometry3D mesh, Point3D[] points,
            char name1, char name2, Vector3D up)
        {
            Point3D point1 = points[ToIndex(name1)];
            Point3D point2 = points[ToIndex(name2)];
            AddSegment(mesh, point1, point2, up);
        }

        // Make a thin rectangular prism between the two points.
        private void AddSegment(MeshGeometry3D mesh, Point3D point1, Point3D point2, Vector3D up)
        {
            const double thickness = 0.01;

            // Get the segment's vector.
            Vector3D v = point2 - point1;

            // Get the scaled up vector.
            Vector3D n1 = ScaleVector(up, thickness / 2.0);

            // Get another scaled perpendicular vector.
            Vector3D n2 = Vector3D.CrossProduct(v, n1);
            n2 = ScaleVector(n2, thickness / 2.0);

            // Make a skinny box.
            // p1pm means point1 PLUS n1 MINUS n2.
            Point3D p1pp = point1 + n1 + n2;
            Point3D p1mp = point1 - n1 + n2;
            Point3D p1pm = point1 + n1 - n2;
            Point3D p1mm = point1 - n1 - n2;
            Point3D p2pp = point2 + n1 + n2;
            Point3D p2mp = point2 - n1 + n2;
            Point3D p2pm = point2 + n1 - n2;
            Point3D p2mm = point2 - n1 - n2;

            // Sides.
            AddTriangle(mesh, p1pp, p1mp, p2mp);
            AddTriangle(mesh, p1pp, p2mp, p2pp);

            AddTriangle(mesh, p1pp, p2pp, p2pm);
            AddTriangle(mesh, p1pp, p2pm, p1pm);

            AddTriangle(mesh, p1pm, p2pm, p2mm);
            AddTriangle(mesh, p1pm, p2mm, p1mm);

            AddTriangle(mesh, p1mm, p2mm, p2mp);
            AddTriangle(mesh, p1mm, p2mp, p1mp);

            // Ends.
            AddTriangle(mesh, p1pp, p1pm, p1mm);
            AddTriangle(mesh, p1pp, p1mm, p1mp);

            AddTriangle(mesh, p2pp, p2mp, p2mm);
            AddTriangle(mesh, p2pp, p2mm, p2pm);
        }

        // Add a box.
        private void AddBox(MeshGeometry3D mesh, Point3D center,
            Vector3D vx, Vector3D vy, Vector3D vz)
        {
            Point3D p111 = center + vx + vy + vz;
            Point3D p110 = center + vx + vy - vz;
            Point3D p101 = center + vx - vy + vz;
            Point3D p100 = center + vx - vy - vz;
            Point3D p011 = center - vx + vy + vz;
            Point3D p010 = center - vx + vy - vz;
            Point3D p001 = center - vx - vy + vz;
            Point3D p000 = center - vx - vy - vz;
            AddRectangle(mesh, p111, p110, p010, p011); // Top
            AddRectangle(mesh, p111, p101, p100, p110); // Right
            AddRectangle(mesh, p011, p010, p000, p001); // Left
            AddRectangle(mesh, p111, p011, p001, p101); // Front
            AddRectangle(mesh, p110, p100, p000, p010); // Back
            AddRectangle(mesh, p000, p100, p101, p001); // Bottom
        }

        // Add a cage.
        private void AddCage(MeshGeometry3D mesh)
        {
            // Top.
            Vector3D up = new Vector3D(0, 1, 0);
            AddSegment(mesh, new Point3D(1, 1, 1), new Point3D(1, 1, -1), up);
            AddSegment(mesh, new Point3D(1, 1, -1), new Point3D(-1, 1, -1), up);
            AddSegment(mesh, new Point3D(-1, 1, -1), new Point3D(-1, 1, 1), up);
            AddSegment(mesh, new Point3D(-1, 1, 1), new Point3D(1, 1, 1), up);

            // Bottom.
            AddSegment(mesh, new Point3D(1, -1, 1), new Point3D(1, -1, -1), up);
            AddSegment(mesh, new Point3D(1, -1, -1), new Point3D(-1, -1, -1), up);
            AddSegment(mesh, new Point3D(-1, -1, -1), new Point3D(-1, -1, 1), up);
            AddSegment(mesh, new Point3D(-1, -1, 1), new Point3D(1, -1, 1), up);

            // Sides.
            Vector3D right = new Vector3D(1, 0, 0);
            AddSegment(mesh, new Point3D(1, -1, 1), new Point3D(1, 1, 1), right);
            AddSegment(mesh, new Point3D(1, -1, -1), new Point3D(1, 1, -1), right);
            AddSegment(mesh, new Point3D(-1, -1, 1), new Point3D(-1, 1, 1), right);
            AddSegment(mesh, new Point3D(-1, -1, -1), new Point3D(-1, 1, -1), right);
        }

        // Set the vector's length.
        private Vector3D ScaleVector(Vector3D vector, double length)
        {
            double scale = length / vector.Length;
            return new Vector3D(
                vector.X * scale,
                vector.Y * scale,
                vector.Z * scale);
        }

        // Adjust the camera's position.
        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.Key)
            {
                case Key.Up:
                    CameraPhi += CameraDPhi;
                    if (CameraPhi > Math.PI / 2.0) CameraPhi = Math.PI / 2.0;
                    break;
                case Key.Down:
                    CameraPhi -= CameraDPhi;
                    if (CameraPhi < -Math.PI / 2.0) CameraPhi = -Math.PI / 2.0;
                    break;
                case Key.Left:
                    CameraTheta += CameraDTheta;
                    break;
                case Key.Right:
                    CameraTheta -= CameraDTheta;
                    break;
                case Key.Add:
                case Key.OemPlus:
                    CameraR -= CameraDR;
                    if (CameraR < CameraDR) CameraR = CameraDR;
                    break;
                case Key.Subtract:
                case Key.OemMinus:
                    CameraR += CameraDR;
                    break;
            }

            // Update the camera's position.
            PositionCamera();
        }

        // Position the camera.
        private void PositionCamera()
        {
            // Calculate the camera's position in Cartesian coordinates.
            double y = CameraR * Math.Sin(CameraPhi);
            double hyp = CameraR * Math.Cos(CameraPhi);
            double x = hyp * Math.Cos(CameraTheta);
            double z = hyp * Math.Sin(CameraTheta);
            TheCamera.Position = new Point3D(x, y, z);

            // Look toward the origin.
            TheCamera.LookDirection = new Vector3D(-x, -y, -z);

            // Set the Up direction.
            TheCamera.UpDirection = new Vector3D(0, 1, 0);
        }

        // Show the selected models.
        private void chkItem_Click(object sender, RoutedEventArgs e)
        {
            MainModelGroup.Children.Clear();

            foreach (Light light in Lights)
                MainModelGroup.Children.Add(light);

            if (chkAxes.IsChecked.Value)
                MainModelGroup.Children.Add(AxesModel);
            if (chkFaces.IsChecked.Value)
            {
                MainModelGroup.Children.Add(IcosaFaceModel);
                MainModelGroup.Children.Add(DedecaFaceModel);
            }
            if (chkEdges.IsChecked.Value)
            {
                MainModelGroup.Children.Add(IcosaEdgeModel);
                MainModelGroup.Children.Add(DedecaEdgeModel);
            }
        }
    }
}
