import math
import numpy

class ScalingMagnet:
    def __init__(self):
        self._b0 = 0.
        self._k = 0.
        self._phi_max = 0.
        self._radius = 0.
        self._coefficients = numpy.zeros((10, 10))

    def set_coefficient_lookup(self):
        for i in range(10):
            const = self._radius**(self._k-i)/math.factorial(i)
            for index in range(0, i):
                const *= (self._k-index)
            self._coefficients[i, 0] = const

    def get_field(self, r, phi, y):
        return (0., self.get_by_field(r, y)*self.get_end_field(phi)*self._b0, 0.)

    def get_by_field(self, r, y):
        by = 0.
        for i in range(10):
            for j in range(10):
                by += self._coefficients[i, j]*(r-self._radius)**i*y**j
        return by


class EndField:
    def __init__(self):
        self._derivatives = numpy.zeros((10, 10))

    def set_derivative_indices():

    def get_derivative_index(n, i):
        """
          d^p f(x) = sum_q[ a_pq {f(x)}^q ]
        """
        return self._derivatives[n, i]

    def get_end_field(self, phi):
        if phi > self._phi_max:
            return 0.
        else:
            return 1.

def main():
    scale = ScalingMagnet()
    scale._k = 1.92
    scale._phi_max = 1.
    scale._radius = 1.
    scale._b0 = 1.
    scale.set_coefficient_lookup()
    print '\n'
    for i in range(10):
        print scale._coefficients[i, 0],
    print '\n'
    for i in range(-10, 11):
        r = 1.-0.08*i
        print r, scale.get_field(r, 0., 0.)[1], scale._b0*r**scale._k

if __name__ == "__main__":
    main()

