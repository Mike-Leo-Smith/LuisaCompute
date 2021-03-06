//
// Created by Mike Smith on 2020/12/2.
//

#pragma once

#include <array>
#include <string_view>

#include <runtime/image.h>
#include <runtime/volume.h>
#include <runtime/buffer.h>
#include <runtime/texture_heap.h>
#include <ast/function_builder.h>

namespace luisa::compute {

namespace detail {
template<typename T>
struct Expr;
}

template<typename T>
struct Var;

template<concepts::scalar T>
[[nodiscard]] inline auto make_vector2(detail::Expr<T> x, detail::Expr<T> y) noexcept;

template<concepts::scalar T>
[[nodiscard]] inline auto make_vector3(detail::Expr<T> x, detail::Expr<T> y, detail::Expr<T> z) noexcept;

template<concepts::scalar T>
[[nodiscard]] inline auto make_vector4(detail::Expr<T> x, detail::Expr<T> y, detail::Expr<T> z, detail::Expr<T> w) noexcept;

namespace detail {

template<typename T>
class ExprBase {

public:
    using ValueType = T;

protected:
    const Expression *_expression;

public:
    explicit ExprBase(const Expression *expr) noexcept : _expression{expr} {}

    template<typename U>
    requires concepts::basic<std::remove_cvref_t<U>>    // to ensure the value can be code-generated as literals
        && concepts::non_pointer<std::remove_cvref_t<U>>// to prevent conversion from pointer to bool
        && std::same_as<T, std::remove_cvref_t<U>> ExprBase(U &&literal)
    noexcept
        : ExprBase{FunctionBuilder::current()->literal(Type::of<U>(), std::forward<U>(literal))} {}

    constexpr ExprBase(ExprBase &&) noexcept = default;
    constexpr ExprBase(const ExprBase &) noexcept = default;
    [[nodiscard]] constexpr auto expression() const noexcept { return _expression; }

#define LUISA_MAKE_EXPR_BINARY_OP(op, op_concept_name, op_tag_name)                      \
    template<typename U>                                                                 \
    requires concepts::op_concept_name<T, U>                                             \
    [[nodiscard]] auto operator op(Expr<U> rhs) const noexcept {                         \
        using R = std::remove_cvref_t<decltype(std::declval<T>() op std::declval<U>())>; \
        return Expr<R>{FunctionBuilder::current()->binary(                               \
            Type::of<R>(),                                                               \
            BinaryOp::op_tag_name, this->expression(), rhs.expression())};               \
    }                                                                                    \
    template<typename U>                                                                 \
    [[nodiscard]] auto operator op(U &&rhs) const noexcept {                             \
        return this->operator op(Expr{std::forward<U>(rhs)});                            \
    }
    LUISA_MAKE_EXPR_BINARY_OP(+, operator_add, ADD)
    LUISA_MAKE_EXPR_BINARY_OP(-, operator_sub, SUB)
    LUISA_MAKE_EXPR_BINARY_OP(*, operator_mul, MUL)
    LUISA_MAKE_EXPR_BINARY_OP(/, operator_div, DIV)
    LUISA_MAKE_EXPR_BINARY_OP(%, operator_mod, MOD)
    LUISA_MAKE_EXPR_BINARY_OP(&, operator_bit_and, BIT_AND)
    LUISA_MAKE_EXPR_BINARY_OP(|, operator_bit_or, BIT_OR)
    LUISA_MAKE_EXPR_BINARY_OP(^, operator_bit_Xor, BIT_XOR)
    LUISA_MAKE_EXPR_BINARY_OP(<<, operator_shift_left, SHL)
    LUISA_MAKE_EXPR_BINARY_OP(>>, operator_shift_right, SHR)
    LUISA_MAKE_EXPR_BINARY_OP(&&, operator_and, AND)
    LUISA_MAKE_EXPR_BINARY_OP(||, operator_or, OR)
    LUISA_MAKE_EXPR_BINARY_OP(==, operator_equal, EQUAL)
    LUISA_MAKE_EXPR_BINARY_OP(!=, operator_not_equal, NOT_EQUAL)
    LUISA_MAKE_EXPR_BINARY_OP(<, operator_less, LESS)
    LUISA_MAKE_EXPR_BINARY_OP(<=, operator_less_equal, LESS_EQUAL)
    LUISA_MAKE_EXPR_BINARY_OP(>, operator_greater, GREATER)
    LUISA_MAKE_EXPR_BINARY_OP(>=, operator_greater_equal, GREATER_EQUAL)
#undef LUISA_MAKE_EXPR_BINARY_OP

    template<typename U>
    requires concepts::operator_access<T, U>
    [[nodiscard]] auto operator[](Expr<U> index) const noexcept {
        using R = std::remove_cvref_t<decltype(std::declval<T>()[std::declval<U>()])>;
        return Expr<R>{FunctionBuilder::current()->access(
            Type::of<R>(),
            this->expression(), index.expression())};
    }

    template<typename U>
    [[nodiscard]] auto operator[](U &&index) const noexcept { return this->operator[](Expr{std::forward<U>(index)}); }

    void operator=(const ExprBase &rhs) &noexcept {
        FunctionBuilder::current()->assign(AssignOp::ASSIGN, this->expression(), rhs.expression());
    }

    void operator=(ExprBase &&rhs) &noexcept {
        FunctionBuilder::current()->assign(AssignOp::ASSIGN, this->expression(), rhs.expression());
    }

#define LUISA_MAKE_EXPR_ASSIGN_OP(op, op_concept_name, op_tag_name)                                      \
    template<typename U>                                                                                 \
    requires concepts::op_concept_name<T, U>                                                             \
    void operator op(Expr<U> rhs) &noexcept {                                                            \
        FunctionBuilder::current()->assign(AssignOp::op_tag_name, this->expression(), rhs.expression()); \
    }                                                                                                    \
    template<typename U>                                                                                 \
    void operator op(U &&rhs) &noexcept {                                                                \
        return this->operator op(Expr{std::forward<U>(rhs)});                                            \
    }
    LUISA_MAKE_EXPR_ASSIGN_OP(=, assignable, ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(+=, add_assignable, ADD_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(-=, sub_assignable, SUB_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(*=, mul_assignable, MUL_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(/=, div_assignable, DIV_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(%=, mod_assignable, MOD_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(&=, bit_and_assignable, BIT_AND_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(|=, bit_or_assignable, BIT_OR_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(^=, bit_xor_assignable, BIT_XOR_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(<<=, shift_left_assignable, SHL_ASSIGN)
    LUISA_MAKE_EXPR_ASSIGN_OP(>>=, shift_right_assignable, SHR_ASSIGN)
#undef LUISA_MAKE_EXPR_ASSIGN_OP

    // casts
    template<typename Dest>
    requires concepts::static_convertible<T, Dest>
    [[nodiscard]] auto cast() const noexcept {
        return Expr<Dest>{FunctionBuilder::current()->cast(Type::of<Dest>(), CastOp::STATIC, _expression)};
    }

    template<typename Dest>
    requires concepts::bitwise_convertible<T, Dest>
    [[nodiscard]] auto as() const noexcept {
        return Expr<Dest>{FunctionBuilder::current()->cast(Type::of<Dest>(), CastOp::BITWISE, _expression)};
    }
};

template<typename T>
struct Expr : public ExprBase<T> {
    using ExprBase<T>::ExprBase;
    Expr(Expr &&another) noexcept = default;
    Expr(const Expr &another) noexcept = default;
    void operator=(Expr &&rhs) noexcept { ExprBase<T>::operator=(rhs); }
    void operator=(const Expr &rhs) noexcept { ExprBase<T>::operator=(rhs); }
};

template<typename... T>
struct Expr<std::tuple<T...>> : public ExprBase<std::tuple<T...>> {
    using ExprBase<std::tuple<T...>>::ExprBase;
    Expr(Expr &&another) noexcept = default;
    Expr(const Expr &another) noexcept = default;
    void operator=(Expr &&rhs) noexcept { ExprBase<std::tuple<T...>>::operator=(rhs); }
    void operator=(const Expr &rhs) noexcept { ExprBase<std::tuple<T...>>::operator=(rhs); }
    template<size_t i>
    [[nodiscard]] auto member() const noexcept {
        using M = std::tuple_element_t<i, std::tuple<T...>>;
        return Expr<M>{FunctionBuilder::current()->member(
            Type::of<M>(), this->expression(), i)};
    };
};

template<typename T>
struct Expr<Vector<T, 2>> : public ExprBase<Vector<T, 2>> {
    using ExprBase<Vector<T, 2>>::ExprBase;
    Expr(Expr &&another) noexcept = default;
    Expr(const Expr &another) noexcept = default;
    void operator=(Expr &&rhs) noexcept { ExprBase<Vector<T, 2>>::operator=(rhs); }
    void operator=(const Expr &rhs) noexcept { ExprBase<Vector<T, 2>>::operator=(rhs); }
    Expr<T> x{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x0u)};
    Expr<T> y{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x1u)};
#include <dsl/swizzle_2.inl.h>
};

template<typename T>
struct Expr<Vector<T, 3>> : public ExprBase<Vector<T, 3>> {
    using ExprBase<Vector<T, 3>>::ExprBase;
    Expr(Expr &&another) noexcept = default;
    Expr(const Expr &another) noexcept = default;
    void operator=(Expr &&rhs) noexcept { ExprBase<Vector<T, 3>>::operator=(rhs); }
    void operator=(const Expr &rhs) noexcept { ExprBase<Vector<T, 3>>::operator=(rhs); }
    Expr<T> x{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x0u)};
    Expr<T> y{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x1u)};
    Expr<T> z{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x2u)};
#include <dsl/swizzle_3.inl.h>
};

template<typename T>
struct Expr<Vector<T, 4>> : public ExprBase<Vector<T, 4>> {
    using ExprBase<Vector<T, 4>>::ExprBase;
    Expr(Expr &&another) noexcept = default;
    Expr(const Expr &another) noexcept = default;
    void operator=(Expr &&rhs) noexcept { ExprBase<Vector<T, 4>>::operator=(rhs); }
    void operator=(const Expr &rhs) noexcept { ExprBase<Vector<T, 4>>::operator=(rhs); }
    Expr<T> x{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x0u)};
    Expr<T> y{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x1u)};
    Expr<T> z{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x2u)};
    Expr<T> w{FunctionBuilder::current()->swizzle(Type::of<T>(), this->expression(), 1u, 0x3u)};
#include <dsl/swizzle_4.inl.h>
};

template<typename>
struct BufferExprAsAtomic {};

template<typename T>
struct Expr<Buffer<T>> : public BufferExprAsAtomic<T> {

public:
    using ValueType = Buffer<T>;

private:
    const RefExpr *_expression{nullptr};

public:
    explicit Expr(const RefExpr *expr) noexcept
        : _expression{expr} {}
    explicit Expr(BufferView<T> buffer) noexcept
        : _expression{FunctionBuilder::current()->buffer_binding(
            Type::of<Buffer<T>>(),
            buffer.handle(), buffer.offset_bytes())} {}

    [[nodiscard]] const RefExpr *expression() const noexcept { return _expression; }

    [[nodiscard]] auto operator[](Expr<uint> i) const noexcept {
        return Expr<T>{FunctionBuilder::current()->access(
            Type::of<T>(), _expression, i.expression())};
    };

    [[nodiscard]] auto operator[](Expr<int> i) const noexcept {
        return Expr<T>{FunctionBuilder::current()->access(
            Type::of<T>(), _expression, i.expression())};
    };
};

template<typename T>
struct Expr<BufferView<T>> : public Expr<Buffer<T>> {
    using Expr<Buffer<T>>::Expr;
};

template<typename T>
class AtomicRef {

private:
    const AccessExpr *_expression{nullptr};

public:
    explicit AtomicRef(const AccessExpr *expr) noexcept
        : _expression{expr} {}

    void store(Expr<T> value) const noexcept {
        FunctionBuilder::current()->call(CallOp::ATOMIC_STORE, {this->_expression, value.expression()});
    }

    [[nodiscard]] auto load() const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_LOAD, {this->_expression});
        return Expr<T>{expr};
    };

    [[nodiscard]] auto exchange(Expr<T> desired) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_EXCHANGE, {this->_expression, desired.expression()});
        return Expr<T>{expr};
    }

    // stores old == compare ? val : old, returns old
    [[nodiscard]] auto compare_exchange(Expr<T> expected, Expr<T> desired) const noexcept {
        auto expr = FunctionBuilder::current()->call(
            Type::of<T>(), CallOp::ATOMIC_COMPARE_EXCHANGE,
            {this->_expression, expected.expression(), desired.expression()});
        return Expr<T>{expr};
    }

    [[nodiscard]] auto fetch_add(Expr<T> val) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_FETCH_ADD, {this->_expression, val.expression()});
        return Expr<T>{expr};
    };

    [[nodiscard]] auto fetch_sub(Expr<T> val) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_FETCH_SUB, {this->_expression, val.expression()});
        return Expr<T>{expr};
    };

    [[nodiscard]] auto fetch_and(Expr<T> val) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_FETCH_AND, {this->_expression, val.expression()});
        return Expr<T>{expr};
    };

    [[nodiscard]] auto fetch_or(Expr<T> val) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_FETCH_OR, {this->_expression, val.expression()});
        return Expr<T>{expr};
    };

    [[nodiscard]] auto fetch_xor(Expr<T> val) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_FETCH_XOR, {this->_expression, val.expression()});
        return Expr<T>{expr};
    };

    [[nodiscard]] auto fetch_min(Expr<T> val) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_FETCH_MIN, {this->_expression, val.expression()});
        return Expr<T>{expr};
    };

    [[nodiscard]] auto fetch_max(Expr<T> val) const noexcept {
        auto expr = FunctionBuilder::current()->call(Type::of<T>(), CallOp::ATOMIC_FETCH_MAX, {this->_expression, val.expression()});
        return Expr<T>{expr};
    };
};

template<>
struct BufferExprAsAtomic<int> {
    [[nodiscard]] auto atomic(Expr<int> i) const noexcept {
        return AtomicRef<int>{FunctionBuilder::current()->access(
            Type::of<int>(),
            static_cast<const Expr<Buffer<int>> *>(this)->expression(),
            i.expression())};
    }
    [[nodiscard]] auto atomic(Expr<uint> i) const noexcept {
        return AtomicRef<int>{FunctionBuilder::current()->access(
            Type::of<int>(),
            static_cast<const Expr<Buffer<int>> *>(this)->expression(),
            i.expression())};
    }
};

template<>
struct BufferExprAsAtomic<uint> {
    [[nodiscard]] auto atomic(Expr<int> i) const noexcept {
        return AtomicRef<uint>{FunctionBuilder::current()->access(
            Type::of<uint>(),
            static_cast<const Expr<Buffer<uint>> *>(this)->expression(),
            i.expression())};
    }
    [[nodiscard]] auto atomic(Expr<uint> i) const noexcept {
        return AtomicRef<uint>{FunctionBuilder::current()->access(
            Type::of<uint>(),
            static_cast<const Expr<Buffer<uint>> *>(this)->expression(),
            i.expression())};
    }
};

template<typename T>
struct Expr<Image<T>> {

public:
    using ValueType = Image<T>;

private:
    const RefExpr *_expression{nullptr};
    const Expression *_offset{nullptr};

    [[nodiscard]] auto _offset_uv(const Expression *uv) const noexcept -> const Expression * {
        if (_offset == nullptr) { return uv; }
        auto f = FunctionBuilder::current();
        return f->binary(Type::of<uint2>(), BinaryOp::ADD, uv, _offset);
    }

public:
    explicit Expr(const RefExpr *expr, const Expression *offset) noexcept
        : _expression{expr}, _offset{offset} {}
    explicit Expr(ImageView<T> image) noexcept
        : _expression{FunctionBuilder::current()->texture_binding(
            Type::of<Image<T>>(), image.handle())},
          _offset{any(image.offset())
                      ? FunctionBuilder::current()->literal(Type::of<uint2>(), image.offset())
                      : nullptr} {}

    [[nodiscard]] const RefExpr *expression() const noexcept { return _expression; }

    [[nodiscard]] auto read(Expr<uint2> uv) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<Vector<T, 4>>{f->call(
            Type::of<Vector<T, 4>>(), CallOp::TEXTURE_READ,
            {_expression, _offset_uv(uv.expression())})};
    };

    void write(Expr<uint2> uv, Expr<Vector<T, 4>> value) const noexcept {
        FunctionBuilder::current()->call(
            CallOp::TEXTURE_WRITE,
            {_expression, _offset_uv(uv.expression()), value.expression()});
    }
};

template<typename T>
struct Expr<ImageView<T>> : public Expr<Image<T>> {
    using Expr<Image<T>>::Expr;
};

template<typename T>
struct Expr<Volume<T>> {

public:
    using ValueType = Volume<T>;

private:
    const RefExpr *_expression{nullptr};
    const Expression *_offset{nullptr};

    [[nodiscard]] auto _offset_uvw(const Expression *uvw) const noexcept -> const Expression * {
        if (_offset == nullptr) { return uvw; }
        auto f = FunctionBuilder::current();
        return f->binary(Type::of<uint3>(), BinaryOp::ADD, uvw, _offset);
    }

public:
    explicit Expr(const RefExpr *expr, const Expression *offset) noexcept
        : _expression{expr}, _offset{offset} {}
    explicit Expr(VolumeView<T> volume) noexcept
        : _expression{FunctionBuilder::current()->texture_binding(
            Type::of<Volume<T>>(), volume.handle())},
          _offset{any(volume.offset())
                      ? FunctionBuilder::current()->literal(Type::of<uint3>(), volume.offset())
                      : nullptr} {}

    [[nodiscard]] const RefExpr *expression() const noexcept { return _expression; }

    [[nodiscard]] auto read(Expr<uint3> uvw) const noexcept {
        return Expr<Vector<T, 4>>{FunctionBuilder::current()->call(
            Type::of<Vector<T, 4>>(), CallOp::TEXTURE_READ,
            {_expression, _offset_uvw(uvw.expression())})};
    };

    void write(Expr<uint3> uvw, Expr<Vector<T, 4>> value) const noexcept {
        FunctionBuilder::current()->call(
            CallOp::TEXTURE_WRITE,
            {_expression, _offset_uvw(uvw.expression()), value.expression()});
    }
};

template<typename T>
struct Expr<VolumeView<T>> : public Expr<Volume<T>> {
    using Expr<Volume<T>>::Expr;
};

struct TextureRef2D {

private:
    const RefExpr *_heap{nullptr};
    const Expression *_index{nullptr};

public:
    TextureRef2D(const RefExpr *heap, const Expression *index) noexcept
        : _heap{heap},
          _index{index} {}

    [[nodiscard]] auto sample(Expr<float2> uv) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_SAMPLE2D,
            {_heap, _index, uv.expression()})};
    }

    [[nodiscard]] auto sample(Expr<float2> uv, Expr<float> mip) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_SAMPLE2D_LEVEL,
            {_heap, _index, uv.expression(), mip.expression()})};
    }

    [[nodiscard]] auto sample(Expr<float2> uv, Expr<float2> dpdx, Expr<float2> dpdy) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_SAMPLE2D_GRAD,
            {_heap, _index, uv.expression(), dpdx.expression(), dpdy.expression()})};
    }

    [[nodiscard]] auto size() const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<uint2>{f->call(
            Type::of<uint2>(), CallOp::TEXTURE_HEAP_SIZE2D,
            {_heap, _index})};
    }

    [[nodiscard]] auto size(Expr<int> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<uint2>{f->call(
            Type::of<uint2>(), CallOp::TEXTURE_HEAP_SIZE2D_LEVEL,
            {_heap, _index, level.expression()})};
    }

    [[nodiscard]] auto size(Expr<uint> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<uint2>{f->call(
            Type::of<uint2>(), CallOp::TEXTURE_HEAP_SIZE2D_LEVEL,
            {_heap, _index, level.expression()})};
    }

    [[nodiscard]] auto read(Expr<uint2> coord) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_READ2D,
            {_heap, _index, coord.expression()})};
    }

    [[nodiscard]] auto read(Expr<uint2> coord, Expr<int> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_READ2D_LEVEL,
            {_heap, _index, coord.expression(), level.expression()})};
    }

    [[nodiscard]] auto read(Expr<uint2> coord, Expr<uint> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_READ2D_LEVEL,
            {_heap, _index, coord.expression(), level.expression()})};
    }
};

struct TextureRef3D {

private:
    const RefExpr *_heap{nullptr};
    const Expression *_index{nullptr};

public:
    TextureRef3D(const RefExpr *heap, const Expression *index) noexcept
        : _heap{heap},
          _index{index} {}

    [[nodiscard]] auto sample(Expr<float3> uvw) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_SAMPLE3D,
            {_heap, _index, uvw.expression()})};
    }

    [[nodiscard]] auto sample(Expr<float3> uvw, Expr<float> mip) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_SAMPLE3D_LEVEL,
            {_heap, _index, uvw.expression(), mip.expression()})};
    }

    [[nodiscard]] auto sample(Expr<float3> uvw, Expr<float3> dpdx, Expr<float3> dpdy) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_SAMPLE3D_GRAD,
            {_heap, _index, uvw.expression(), dpdx.expression(), dpdy.expression()})};
    }

    [[nodiscard]] auto size() const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<uint3>{f->call(
            Type::of<uint3>(), CallOp::TEXTURE_HEAP_SIZE3D,
            {_heap, _index})};
    }

    [[nodiscard]] auto size(Expr<int> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<uint3>{f->call(
            Type::of<uint3>(), CallOp::TEXTURE_HEAP_SIZE3D_LEVEL,
            {_heap, _index, level.expression()})};
    }

    [[nodiscard]] auto size(Expr<uint> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<uint3>{f->call(
            Type::of<uint3>(), CallOp::TEXTURE_HEAP_SIZE3D_LEVEL,
            {_heap, _index, level.expression()})};
    }

    [[nodiscard]] auto read(Expr<uint3> coord) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_READ3D,
            {_heap, _index, coord.expression()})};
    }

    [[nodiscard]] auto read(Expr<uint3> coord, Expr<int> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_READ3D_LEVEL,
            {_heap, _index, coord.expression(), level.expression()})};
    }

    [[nodiscard]] auto read(Expr<uint3> coord, Expr<uint> level) const noexcept {
        auto f = FunctionBuilder::current();
        return Expr<float4>{f->call(
            Type::of<float4>(), CallOp::TEXTURE_HEAP_READ3D_LEVEL,
            {_heap, _index, coord.expression(), level.expression()})};
    }
};

template<>
struct Expr<TextureHeap> {

public:
    using ValueType = TextureHeap;

private:
    const RefExpr *_expression{nullptr};

public:
    explicit Expr(const RefExpr *expr) noexcept
        : _expression{expr} {}

    explicit Expr(const TextureHeap &heap) noexcept
        : _expression{FunctionBuilder::current()->texture_heap_binding(heap.handle())} {}
    [[nodiscard]] auto expression() const noexcept { return _expression; }
    [[nodiscard]] auto tex2d(Expr<int> index) const noexcept { return TextureRef2D{_expression, index.expression()}; }
    [[nodiscard]] auto tex2d(Expr<uint> index) const noexcept { return TextureRef2D{_expression, index.expression()}; }
    [[nodiscard]] auto tex3d(Expr<int> index) const noexcept { return TextureRef3D{_expression, index.expression()}; }
    [[nodiscard]] auto tex3d(Expr<uint> index) const noexcept { return TextureRef3D{_expression, index.expression()}; }
};

// deduction guides
template<typename T>
Expr(Expr<T>) -> Expr<T>;

template<concepts::basic T>
Expr(T) -> Expr<T>;

template<typename T>
Expr(const Buffer<T> &) -> Expr<Buffer<T>>;

template<typename T>
Expr(BufferView<T>) -> Expr<Buffer<T>>;

template<typename T>
Expr(const Image<T> &) -> Expr<Image<T>>;

template<typename T>
Expr(ImageView<T>) -> Expr<Image<T>>;

template<typename T>
Expr(const Volume<T> &) -> Expr<Volume<T>>;

template<typename T>
Expr(VolumeView<T>) -> Expr<Volume<T>>;

Expr(const TextureHeap &) -> Expr<TextureHeap>;

template<typename T>
[[nodiscard]] inline const Expression *extract_expression(T &&v) noexcept {
    Expr expr{std::forward<T>(v)};
    return expr.expression();
}

template<typename T>
struct expr_value_impl {
    using type = T;
};

template<typename T>
struct expr_value_impl<Expr<T>> {
    using type = T;
};

template<typename T>
struct expr_value_impl<Var<T>> {
    using type = T;
};

template<typename T>
using expr_value = expr_value_impl<std::remove_cvref_t<T>>;

template<typename T>
using expr_value_t = typename expr_value<T>::type;

template<typename T>
struct is_expr : std::false_type {};

template<typename T>
struct is_expr<Expr<T>> : std::true_type {};

template<typename T>
struct is_expr<Var<T>> : std::true_type {};

template<typename T>
constexpr auto is_expr_v = is_expr<T>::value;

}// namespace detail

template<typename I>
detail::TextureRef2D TextureHeap::tex2d(I &&index) const noexcept {
    return detail::Expr<TextureHeap>{*this}.tex2d(std::forward<I>(index));
}

template<typename I>
detail::TextureRef2D TextureHeap::tex3d(I &&index) const noexcept {
    return detail::Expr<TextureHeap>{*this}.tex3d(std::forward<I>(index));
}

}// namespace luisa::compute

#define LUISA_MAKE_GLOBAL_EXPR_UNARY_OP(op, op_concept, op_tag)                            \
    template<luisa::concepts::op_concept T>                                                \
    [[nodiscard]] inline auto operator op(luisa::compute::detail::Expr<T> expr) noexcept { \
        using R = std::remove_cvref_t<decltype(op std::declval<T>())>;                     \
        return luisa::compute::detail::Expr<R>{                                            \
            luisa::compute::detail::FunctionBuilder::current()->unary(                     \
                luisa::compute::Type::of<R>(),                                             \
                luisa::compute::UnaryOp::op_tag,                                           \
                expr.expression())};                                                       \
    }
LUISA_MAKE_GLOBAL_EXPR_UNARY_OP(+, operator_plus, PLUS)
LUISA_MAKE_GLOBAL_EXPR_UNARY_OP(-, operator_minus, MINUS)
LUISA_MAKE_GLOBAL_EXPR_UNARY_OP(!, operator_not, NOT)
LUISA_MAKE_GLOBAL_EXPR_UNARY_OP(~, operator_bit_not, BIT_NOT)
#undef LUISA_MAKE_GLOBAL_EXPR_UNARY_OP

#define LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(op, op_concept)                          \
    template<typename Lhs, typename Rhs>                                          \
    requires luisa::concepts::basic<Lhs> && luisa::concepts::op_concept<Lhs, Rhs> \
    [[nodiscard]] inline auto                                                     \
    operator op(Lhs lhs, luisa::compute::detail::Expr<Rhs> rhs) noexcept {        \
        return luisa::compute::detail::Expr{lhs} op rhs;                          \
    }
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(+, operator_add)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(-, operator_sub)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(*, operator_mul)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(/, operator_div)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(%, operator_mod)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(&, operator_bit_and)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(|, operator_bit_or)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(^, operator_bit_Xor)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(<<, operator_shift_left)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(>>, operator_shift_right)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(&&, operator_and)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(||, operator_or)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(<, operator_less)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(<=, operator_less_equal)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(>, operator_greater)
LUISA_MAKE_GLOBAL_EXPR_BINARY_OP(>=, operator_greater_equal)
#undef LUISA_MAKE_GLOBAL_EXPR_BINARY_OP
