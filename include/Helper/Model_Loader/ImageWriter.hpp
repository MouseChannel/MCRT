#include "Wrapper/Image.hpp"

namespace MCRT {

class ImageWriter {
public:
    static void WriteImage(std::shared_ptr<Image> image);
    static void WriteCubemap(std::shared_ptr<Image> cubemap);
};
}
