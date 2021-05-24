class OperatorLogClass {
  public:
  std::string name{};
  char* data{nullptr};
  std::string ToString() const {
    return "OperatorLogClass:" + name + "\t@" + std::to_string((intptr_t)this);
  }
  OperatorLogClass(){LOG_DEBUG(ToString())};
  OperatorLogClass(const std::string& _name) {
    name = _name;
    LOG_DEBUG(ToString())
  }
  OperatorLogClass(const OperatorLogClass& obj) {
    name = obj.name;
    LOG_DEBUG("this: " << ToString() << "\tobj: " << obj.ToString())
  };
  OperatorLogClass(OperatorLogClass&& obj) {
    LOG_DEBUG("\tobj: " << obj.ToString())
    std::swap(name, obj.name);
    LOG_DEBUG("this: " << ToString())
  };
  virtual ~OperatorLogClass(){LOG_DEBUG(ToString())};
  OperatorLogClass& operator=(const OperatorLogClass& obj) {
    LOG_DEBUG("this: " << ToString() << "\tobj: " << obj.ToString())
    name = obj.name;
    LOG_DEBUG("this: " << ToString() << "\tobj: " << obj.ToString())
    return *this;
  };

  OperatorLogClass& operator=(OperatorLogClass&& obj) {
    LOG_DEBUG("\tobj: " << obj.ToString())
    std::swap(name, obj.name);
    LOG_DEBUG("this: " << ToString())
    return *this;
  };
};