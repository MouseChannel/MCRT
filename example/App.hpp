#include <memory>

namespace MCRT {
class ImGuiContext;
class Window;
// class Context;
class App {
public:
  App() = default;
  void init();
  void run();

private:
  std::shared_ptr<Window> window;

  std::unique_ptr<ImGuiContext> imgui;
};
} // namespace MCRT