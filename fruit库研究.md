
fruit依赖注入框架研究
====

https://github.com/google/fruit/wiki

// 编译：
// C++11
// 目前不支持Windows，因为VS有bug
https://github.com/google/fruit/wiki/install

https://www.martinfowler.com/articles/injection.html
https://www.codeproject.com/articles/29271/design-pattern-inversion-of-control-and-dependency
http://blog.csdn.net/yqj2065/article/details/8510074




## 依赖注入

模块化服务单元，通过外部设置参数的形式传递给对象，而不是对象主动查找服务

dependency injection is a technique whereby one object supplies the dependencies of another object. A dependency is an object that can be used (a service). An injection is the passing of a dependency to a dependent object (a client) that would use it. The service is made part of the client's state.[1] Passing the service to the client, rather than allowing a client to build or find the service, is the fundamental requirement of the pattern.

This fundamental requirement means that using values (services) produced within the class from new or static methods is prohibited. The class should accept values passed in from outside.

The intent behind dependency injection is to decouple objects to the extent that no client code has to be changed simply because an object it depends on needs to be changed to a different one.

Dependency injection is one form of the broader technique of inversion of control. Rather than low level code calling up to high level code, high level code can receive lower level code that it can call down to. This inverts the typical control pattern seen in procedural programming.


using fruit::Injector;


## 组件化

using fruit::Component;



## 类似java的AOP注解

annotated 

```
  /**
   * Binds the base class (typically, an interface or abstract class) I to the implementation C.
   * 
   * This supports annotated injection, just wrap I and/or C in fruit::Annotated<> if desired.
   */
  template<typename I, typename C>
  PartialComponent<fruit::impl::Bind<I, C>, Bindings...> bind();
```

### constructor injection


### lambda injection




