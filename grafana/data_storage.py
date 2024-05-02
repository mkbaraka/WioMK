class DataStorage():
    def __init__(self):
        self.avg_data = 0
        self.n = 0
        self.max = float('-inf')
        self.min = float('inf')
    
    def add_value(self, value):
        self.avg_data += value
        self.n += 1
    
    def pub_metrics(self, client, str: topic):
        avg = self.avg_data / self.n
        values = [avg, self.max, self.min]
        topics_prefixes = ["average/", "maximum/", "minimum/"]

        for value, prefix in zip(values, topics_prefixes):
            encoded = str(value).encode('utf-8')
            client.publish(prefix + topic, encoded)
