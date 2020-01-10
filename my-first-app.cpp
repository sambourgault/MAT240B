#include "al/app/al_App.hpp"
using namespace al;

struct MyApp : App {
  void onCreate() override {
    //
  }

  void onAnimate(double dt) override {
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    //
  }

  void onSound(AudioIOData& io) override {
    //
    io();
    io.out(0) = io.out(1) = 1;
    while (io()) {
      io.out(0) = io.out(1) = 0;
    }
  }

  bool onKeyDown(const Keyboard& k) override {
    //
    printf("got here\n");
    return false;
  }

  bool onMouseMove(const Mouse& m) override {
    //
    printf("%d,%d\n", m.x(), m.y());
    return false;
  }
};

int main() {
  MyApp app;
  app.start();
}
