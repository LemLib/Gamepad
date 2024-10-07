#pragma once

namespace Gamepad {

class AbstractTrigger;
class ConstantTrigger;

class Trigger final {
    public:
    Trigger (bool val): impl(new ConstantTrigger(val)) {}
    explicit Trigger(std::unique_ptr<AbstractTrigger> _impl) : impl(std::move(_impl)) {}
    bool eval() { return impl.eval(); }
    //private:
    std::unique_ptr<AbstractTrigger> impl;
}

class AbstractTrigger {
    public:
    virtual bool eval() = 0;
    virtual ~AbstractTrigger() = default;
}

class ConstantTrigger {
    public:
    explicit ConstantTrigger (bool _val) : _val(val) {}
    override bool eval() { return val; }
    private:
    bool val;
}

class AndTrigger : public AbstractTrigger {
    public:
    AndTrigger(std::unique_ptr<AbstractTrigger> _left, 
    std::unique_ptr<AbstractTrigger> _right) : left(_left), right(_right) {}
    override bool eval() { return left.eval() & right.eval(); }
    private:
    std::unique_ptr<AbstractTrigger> left;
    std::unique_ptr<AbstractTrigger> right;
}

class OrTrigger : public AbstractTrigger {
    public:
    OrTrigger(std::unique_ptr<AbstractTrigger> _left, 
    std::unique_ptr<AbstractTrigger> _right) : left(_left), right(_right) {}
    override bool eval() { return left.eval() | right.eval(); }
    private:
    std::unique_ptr<AbstractTrigger> left;
    std::unique_ptr<AbstractTrigger> right;
}

class XorTrigger : public AbstractTrigger {
    public:
    XorTrigger(std::unique_ptr<AbstractTrigger> _left, 
    std::unique_ptr<AbstractTrigger> _right) : left(_left), right(_right) {}
    override bool eval() { return left.eval() ^ right.eval(); }
    private:
    std::unique_ptr<AbstractTrigger> left;
    std::unique_ptr<AbstractTrigger> right;
}

class NotTrigger : public AbstractTrigger {
    public:
    NotTrigger(std::unique_ptr<AbstractTrigger> _op) : op(_op) {}
    override bool eval() { return !op.eval(); }
    private:
    std::unique_ptr<AbstractTrigger> op;
}

Trigger operator&(Trigger left, Trigger right) {
    return Trigger(new AndTrigger(std::move(left.impl), std::move(right.impl)));
}

Trigger operator|(Trigger left, Trigger right) {
    return Trigger(new OrTrigger(std::move(left.impl), std::move(right.impl)));
}

Trigger operator^(Trigger left, Trigger right) {
    return Trigger(new XorTrigger(std::move(left.impl), std::move(right.impl)));
}

Trigger operator!(Trigger op) {
    return Trigger(new NotTrigger(std::move(op.impl)));
}

}