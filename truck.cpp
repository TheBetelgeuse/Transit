class Truck {
 public:

  Truck(int speed, int weight, bool position);
  int GetSpeed() const { return speed_; }
  int GetWeight() const { return weight_; }
  bool GetPosition() const { return position_; }

 private:
  int speed_;
  int weight_;
  bool position_; // До фабрики – 1, обратно – 0
};


Truck::Truck(int speed, int weight, bool position) {
  speed_ = speed;
  weight_ = weight;
  position_ = position;
}
