.func feholdexcept
#include <fenv.h>
int feholdexcept( fenv_t *__envp );
.ixfunc2 'Floating Point Environment' &func
.funcend
.*
.desc begin
The &func function saves the current floating-point environment in the object
pointed to by envp, clears the floating-point status flags, and then installs a non-stop
(continue on floating-point exceptions) mode, if available, for all floating-point
exceptions.
.desc end
.*
.return begin
The &func function returns zero if and only if non-stop floating-point
exception handling was successfully installed.
.return end
.*
.see begin
.seelist &function. fegetenv fesetenv feupdateenv
.see end
.*
.exmp begin
#include <fenv.h>
.exmp break
void main( void )
{
    fenv_t env;
    feholdexcept( &env );
}
.exmp end
.class C99
.system
