#ifndef BACKENDMODE_H_
#define BACKENDMODE_H_

enum class BackendMode
{
    EXECUTOR, DEBUGGER, CONVERTER, COMPILER
};

constexpr BackendMode EXECUTOR  = BackendMode::EXECUTOR;
constexpr BackendMode DEBUGGER  = BackendMode::DEBUGGER;
constexpr BackendMode CONVERTER = BackendMode::CONVERTER;
constexpr BackendMode COMPILER  = BackendMode::COMPILER;

#endif /* BACKENDMODE_H_ */
