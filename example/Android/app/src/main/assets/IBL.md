## Image Based Lighting(IBL)

IBL�������������

������Ⱦ����

$$
L_o(p,w_o) = \int_\Omega (k_d\frac{c}{\pi}+k_s \frac{DFG}{4(w_o \cdot n)(w_i \cdot n )} L_i(p,w_i)(n\cdot w_i) dw_i\\ \\w_o = V = �۲ⷽ�� \\ w_i = L = ���䷽��\\n = ���߷���
$$

$$
L_o(p,w_o) = \int_\Omega L_i(p,w_i)dw_i  f_r(p,w_i,w_o)(n\cdot w_i)dw_i
$$

after split sum approximation

$$
L_o(p,w_o) = \int_\Omega L_i(p,w_i)dw_i*\int_\Omega f_r(p,w_i,w_o)(n\cdot w_i)dw_i

$$

ǰ��ֱ�Ӵӻ�������ͼskybox���ȡ��������Ҫ��Ԥ�������һ��

$$
let: f = \frac{DG}{4(w_o \cdot n )(w_i \cdot n)} = \frac{f_r(p,w_i,w_o)}{F}
$$

$$
\int_\Omega f_r(p,w_i,w_o)(n\cdot w_i)dw_i \\= \int_\Omega \frac{DGF}{4(w_o \cdot n )(w_i \cdot n)}(n\cdot w_i)dw_i \\= \int_\Omega  Ff(n\cdot w_i)dw_i
$$

 $F$ with the Fresnel-Schlick approximation $F =F_0+(1 - F_0)(1 - w_o \cdot h)^5 $

$$
\int_\Omega  \left[ F_0+(1 - F_0)(1 - w_o \cdot h)^5\right] f(n\cdot w_i)dw_i \\ = \int_\Omega F_of(n\cdot w_i)dw_i+ \int_\Omega (1 - F_0)(1 - w_o \cdot h)^5f(n\cdot w_i)dw_i\\ = F_o\int_\Omega f(n\cdot w_i)(1 - (1-w_o\cdot h)^5)dw_i + \int_\Omega(1 - w_o\cdot h)^5f(n \cdot w_i)dw_i \\ = F_oA+B
$$

extract $F_o$ successfully, and we need pre-compute $A$ and $B$
