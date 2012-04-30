#ifndef ICONVERTER_H
#define ICONVERTER_H

template <typename INPUT, typename OUTPUT>
class IConverter {
public:
  virtual ~IConverter() {}

public:
  virtual OUTPUT* operator()(const INPUT*) const = 0;
  virtual OUTPUT* convert   (const INPUT*) const = 0;
};

#endif
