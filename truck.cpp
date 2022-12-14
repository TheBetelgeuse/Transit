class Truck {
 public:

  Truck(bool init_location, int number, int weight, int speed, int lenght);
  int GetSpeed() const { return speed_; }
  int GetWeight() const { return weight_; }
  bool GetPosition() const { return position_; }

 private:
  int speed_;
  int weight_;
  bool position_; // До фабрики – 1, обратно – 0
  int index_;
};


