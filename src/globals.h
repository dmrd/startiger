
struct Camera {
  R3Point eye;
  R3Vector towards;
  R3Vector right;
  R3Vector up;
  double xfov, yfov;
  double neardist, fardist;
};
