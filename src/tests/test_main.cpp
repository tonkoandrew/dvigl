#define JC_TEST_IMPLEMENTATION
#include <jctest/jc_test.h>

#include <dvigl/camera_node.h>

TEST(MyTest, CameraNodeTest)
{
    CameraNode * c = new CameraNode();
    glm::mat4 default_view_mat = c->get_view_matrix();

    const float* actual =(const float*) glm::value_ptr(default_view_mat);
    glm::mat4 v = glm::mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0,-1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    const float* expected =(const float*) glm::value_ptr(v);
    // LOG("\nActual %s\n", glm::to_string(default_view_mat).c_str());
    // LOG("expected %s\n", glm::to_string(v).c_str());
    for(auto i=0; i<16; i++){
        ASSERT_NEAR(actual[i], expected[i], 0.000001f);
    }
    delete c;
}

TEST(MyTest, Division)
{
    ASSERT_EQ(2, 4 / 2);
}

int main(int argc, char *argv[])
{
    jc_test_init(&argc, argv);
    return jc_test_run_all();
}