"""
test_cpp_style can allow exceptions in certain circumstances:

* exceptions dict is a dict like 
  exceptions = { 
    file_name:[
      ("text of line where exception occurs", "reason for exception", "name")
    ]
  }
  I enumerate exceptions using the line where the exception occurs so that if
  someone moves code around the exception is still valid. If the actual line 
  where the exception occurs changes, we have to re-enter the exception.

  Please use these exceptions in moderation - i.e. try to work within the style
  guide wherever possible. In general, we make exceptions only when there is a
  good technical reason (typically a non-compliant interface with an external
  library).

  Please comment up why you think the exception is valid. If there is no  good
  reason for the exception, it may get deleted...
"""

import os

exceptions = { #pylint: disable=C0103
}

CPP_CM = os.path.join('src','common_cpp')
LEGACY = os.path.join('src','LEGACY')
TST = os.path.join('tests', 'cpp_unit')

exceptions[os.path.join(CPP_CM, 'Simulation',
                                           'MAUSPrimaryGeneratorAction.hh')] = [
('    long int seed;', 'CLHEP also uses long int for its random seed - if we want to be compatible we should follow their lead', 'rogers'), #pylint: disable=C0301
]

exceptions[os.path.join(CPP_CM, 'Maths','Complex.hh')] = [
('MAUS::complex& operator+=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
('MAUS::complex& operator-=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
('MAUS::complex& operator*=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
('MAUS::complex& operator/=(MAUS::complex& lhs, const double         rhs);',
 'cpplint confused by MAUS::complex not a class but has math operators',
 'lane'),
]

exceptions[os.path.join(CPP_CM, 'Maths','HermitianMatrix.hh')] = [
('  HermitianMatrix(const Matrix<complex>& original_instance);',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(CPP_CM, 'Maths','Matrix.hh')] = [
('  Matrix(const Matrix<double>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane'),
('  Matrix(const Matrix<complex>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(CPP_CM, 'Maths','Vector.hh')] = [
('  Vector(const Vector<double>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane'),
('  Vector(const Vector<complex>& original_instance)',
 'cpplint confused by STL copy constructor', 'lane')
]

exceptions[os.path.join(TST, 'Maths', 'PolynomialVectorTest.cc')] = [
('  } catch (Squeal squee) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(TST, 'Maths', 'VectorTest.cc')] = [
('  } catch (Squeal squeal) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(TST, 'Optics', 'PhaseSpaceVectorTest.cc')] = [
('  } catch (Squeal squeal) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(TST, 'Optics', 'CovarianceMatrixTest.cc')] = [
('  } catch (Squeal squeal) {}',
 '"catch" is a keyword like "for" and "if", not a function', 'lane')
]

exceptions[os.path.join(CPP_CM, 'JsonCppStreamer','ORStream.hh')] = [
('template<>           orstream& orstream::operator<< <short> (short&  d) {', 'doesnt like the necessary specialisation', 'richards'),
('template<>           orstream& orstream::operator<< <long>  (long&   d) {', 'doesnt like the necessary specialisation', 'richards'),
('template<>           orstream& orstream::operator<< <short> (short* &  d) {', 'doesnt like the necessary specialisation', 'richards'),
('template<>           orstream& orstream::operator<< <long>  (long* &   d) {', 'doesnt like the necessary specialisation', 'richards'),

]

exceptions[os.path.join(CPP_CM, 'JsonCppStreamer','ORStream.cc')] = [
('  strcpy(m_branchName, "");', '', 'richards'),
('  strcpy(m_branchName, "");', '', 'richards'),

]
exceptions[os.path.join(TST, 'JsonCppStreamer','ORStreamTest.cc')] = [
('  strcpy(os.m_branchName, "");', '', 'richards'),
('  strcpy(os.m_branchName, "Test");', '', 'richards'),
('  strcpy(os.m_branchName, "");', '', 'richards'),
('  strcpy(os.m_branchName, "HelloWorld");', '', 'richards'),
('  strcpy(os.m_branchName, "");', '', 'richards'),
('  long nevent = 0;', 'root uses longs', 'richards'),
]
exceptions[os.path.join(CPP_CM, 'JsonCppStreamer','IRStream.hh')] = [
('template<>           irstream& irstream::operator>> <short> (short&  d) {', 'doesnt like the necessary specialisation', 'richards'),
('template<>           irstream& irstream::operator>> <long>  (long&   d) {', 'doesnt like the necessary specialisation', 'richards'),
('template<>           irstream& irstream::operator>> <short> (short* &  d) {', 'doesnt like the necessary specialisation', 'richards'),
('template<>           irstream& irstream::operator>> <long>  (long* &   d) {', 'doesnt like the necessary specialisation', 'richards'),

]

exceptions[os.path.join(CPP_CM, 'JsonCppStreamer','IRStream.cc')] = [
('  long nextEvent = irs.m_tree->GetReadEntry() + 1;', 'root can return a long', 'richards'),
('  strcpy(m_branchName, "");', '', 'richards'),
('  strcpy(m_branchName, "");', '', 'richards')

]

exceptions[os.path.join(TST, 'JsonCppStreamer','IRStreamTest.cc')] = [
('  strcpy(is.m_branchName, "Test");', '', 'richards'),
('  strcpy(is.m_branchName, "");', '', 'richards'),
('  strcpy(is.m_branchName, "");', '', 'richards'),

]

exceptions[os.path.join(CPP_CM, 'JsonCppStreamer','RStream.hh')] = [
('  long m_evtCount;', 'root can return a long', 'richards'),
('    strcpy(m_branchName, "");', '', 'richards'),
('    strcpy(m_branchName, "");', '', 'richards'),
('  strcpy(m_branchName, "");', '', 'richards'),
('    strcpy(m_branchName, "");', '', 'richards'),
('    strcpy(m_branchName, "");', '', 'richards'),
('  strcpy(m_branchName, "");', '', 'richards'),

]

exceptions[os.path.join(CPP_CM, 'JsonCppStreamer','RStream.cc')] = [
('  strcpy(m_branchName, "");', '', 'richards'),
('  strcpy(rs.m_branchName, name);', '', 'richards'),


]

exceptions[os.path.join(TST, 'JsonCppStreamer','RStreamTest.cc')] = [
('    strcpy(t.m_branchName, "TestBranch1");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch1");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch2");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch2");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch1");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch2");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch1");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch1");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch2");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch2");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch1");', '', 'richards'),
('    strcpy(t.m_branchName, "TestBranch2");', '', 'richards'),

]
