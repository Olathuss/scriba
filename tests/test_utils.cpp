#include "test_utils.h"

namespace testutils {
    ObjectTypeInfo& make_test_object_type()
    {
        static ObjectTypeInfo info;
        static bool initialized = false;

        if (!initialized) {
            initialized = true;

            PropertyInfo propx;
            propx.getter = [](void* instance) {
                TestObject* obj = reinterpret_cast<TestObject*>(instance);
                return Value((double)obj->x);
                };
            propx.setter = [](void* instance, const Value& value) {
                TestObject* obj = reinterpret_cast<TestObject*>(instance);
                obj->x = value.is_number() ? (int)value.as_number() : -1;
                };

            PropertyInfo propy;
            propy.getter = [](void* instance) {
                TestObject* obj = reinterpret_cast<TestObject*>(instance);
                return Value((double)obj->y);
                };
            propy.setter = [](void* instance, const Value& value) {
                TestObject* obj = reinterpret_cast<TestObject*>(instance);
                obj->y = value.is_number() ? (int)value.as_number() : -1;
                };

            MethodInfo method;
            method.invoke = [](void* instance, const std::vector<Value>& values) {
                TestObject* obj = reinterpret_cast<TestObject*>(instance);
                return Value((double)obj->add(values[0].as_number(), values[1].as_number()));
                };

            info.name = "test";
            info.properties.emplace("x", propx);
            info.properties.emplace("y", propy);
            info.methods.emplace("add", method);
        }

        return info;
    }

	ObjectRef make_test_object(int x, int y)
	{
		ObjectRef ref;
		ref.instance = new TestObject(x, y);
		ref.type = &make_test_object_type();

		return ref;
	}

	Environment make_env_with_test_object()
	{
		Environment env;
		ObjectRef test_ref = make_test_object();
		env.set("test", Value(test_ref));
		ObjectRef other_ref = make_test_object(1, 2);
		env.set("other_test", Value(other_ref));

        return env;
	}
} // namespace testutils