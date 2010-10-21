from FetchConfiguration import *

def test_run_number():
    try:
        f = FetchConfiguration()
    except:
        assert True
    assert False
    
def test_run_number():
    try:
        f = FetchConfiguration(1234)
    except:
        assert True
    assert False

    assert InitializeDatastructure() is None

